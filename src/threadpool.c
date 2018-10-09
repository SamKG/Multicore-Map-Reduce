/**
 * THREAD POOL LIBRARY
 * Written by: Samyak K. Gupta for CS416-F18
Responsibilities: To allow an arbitrary number of worker threads to work on a common task
 */
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue.h>
#include <pthread.h>
#include <sharedmem.h>
#include <sys/mman.h>
#include <threadpool.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <wordcount.h>
#include <semaphore.h>

const int THREAD_POOL_SIZE = sizeof(ThreadPool);


/**
 * Returns a pointer to a new ThreadPool, allocated in shared memory space. If it already exists, returns that instead.
 *
 *Inputs:
 *	name	-	The name to assign to the thread pool
 *	
 */
ThreadPool* new_thread_pool(char* name, int number_workers){
	printf("INITIALIZING TPOOL\n");
	char* new_name = append_string(name,"_TPOOL");
	int exists_flag = 0;
	int shared_fd = get_shared_fd(new_name,THREAD_POOL_SIZE,&exists_flag);

	/* Initialize space for thread pool */
	ThreadPool* pool = (ThreadPool*) mmap(NULL, THREAD_POOL_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_fd, 0);

	/* If it already exists, we don't need to reinitialize stuff */
	if (exists_flag){
		printf("POOL ALREADY EXISTS!\n");
		goto cleanup;
	}

	pthread_mutexattr_init(&(pool->mutex_attr));
	pthread_mutexattr_setpshared(&(pool->mutex_attr), PTHREAD_PROCESS_SHARED);

	/* Initialize mutex (note - we need to set shared memory attr in mutex) */
	pthread_mutex_init(&(pool->mutex), &(pool->mutex_attr));

	/* Lock the mutex while we initialize the queue */
	pthread_mutex_lock(&(pool->mutex));

	strcpy(pool->name,new_name);
	pool->parameter_queue = new_queue(name);
	pool->thread_count = 0;
	pool->num_running_workers = 0;
	pool->running = 1;

	while (pool->thread_count < number_workers){
		printf("\tINIT THREAD %d\n",pool->thread_count);
		pthread_create(&(pool->threads[pool->thread_count]),NULL,&start_thread_worker,(void*)pool);
		pool->thread_count++;
	}


	/* Done initializing; We can now return the queue. */
	pthread_mutex_unlock(&(pool->mutex));

cleanup:

	free(new_name);
	printf("DONE INITIALIZING TPOOL (%d WORKERS CREATED)\n",pool->thread_count);
	return pool;
}

void destroy_thread_pool(ThreadPool* pool){
	printf("DESTROYING THREAD POOL\n");
	pool->running = 0;
	destroy_queue(pool->parameter_queue);
	shm_unlink(pool->name);;
	munmap(pool,THREAD_POOL_SIZE);
	printf("DONE DESTROYING POOL\n");
}
void* start_thread_worker(void* pool_ptr){
	ThreadPool* pool = (ThreadPool*) pool_ptr;
	struct timespec tm;
	pthread_t tid = pthread_self();
	while(1){
		tm.tv_sec = time(NULL) + 1;
		sem_wait(&(pool->parameter_queue->semaphore_lock));
		Node instruction = queue_dequeue(pool->parameter_queue);
		switch(instruction.operation){
			case Map:
				printf("RECEIVED MAP INSTRUCTION (%d remain in queue)\n\t\t",pool->parameter_queue->count);
				printf("\tINSTRUCTION DATA: num_chunks: %d\n",instruction.num_chunks);
				int num_keys = 0;
				KeyValue* vals = map(instruction.data_offset,instruction.num_chunks,&num_keys);
				//insert into final return array
				KeyValue* ret_arr = (KeyValue*) (general_shm_ptr + pool->return_array_offset + pool->return_array_count*sizeof(KeyValue));
				pool->return_array_count+=num_keys;
				for (int i = 0 ; i < num_keys ; i++){
					ret_arr[i] = vals[i];
				}
				printf("FINISH WITH INSTRUCTION\n");
				free(vals);
				break;
			case Reduce:
				printf("RECEIVED REDUCE INSTRUCTION\n");
				printf("\tINSTRUCTION DATA: num_chunks: %d\n",instruction.num_chunks);
				char* final_str = reduce(instruction.data_offset,instruction.num_chunks);
				int return_off = instruction.meta;
				int str_off = shm_get_general(strlen(final_str)+1);
				char* buff = (char*) (general_shm_ptr + str_off);
				strcpy(buff,final_str);	
				DataChunk* dc = (DataChunk*) (general_shm_ptr + return_off);
				dc->data = str_off;
				free(final_str);
				break;
			case Error:
				//printf("RECEIVED ERROR: EMPTY NODE\n");
				break;
		}
	}

	pthread_exit(NULL);
}
