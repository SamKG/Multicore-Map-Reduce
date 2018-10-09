/**
* PROCESS POOL LIBRARY
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: To allow an arbitrary number of worker processes to work on a common task
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
#include <processpool.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>


const int PROCESS_POOL_SIZE = sizeof(ProcessPool);


/**
* Returns a pointer to a new ProcessPool, allocated in shared memory space. If it already exists, returns that instead.
*
*Inputs:
*	name	-	The name to assign to the process pool
*	
*/
ProcessPool* new_process_pool(char* name, int number_workers){
	printf("INITIALIZING POOL\n");
	char* new_name = append_string(name,"_POOL");
        int exists_flag = 0;
	int shared_fd = get_shared_fd(new_name,PROCESS_POOL_SIZE,&exists_flag);

	/* Initialize space for process pool */
	ProcessPool* pool = (ProcessPool*) mmap(NULL, PROCESS_POOL_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_fd, 0);
	
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
	pool->process_count = 0;
	pool->num_running_workers = 0;
	pool->running = 1;

	while (pool->process_count < number_workers){
		errno = 0;
		int pid = fork();
		if (pid == 0){
			printf("\tWORKER %d\n",getpid());			// Handle child process
			start_worker(pool);
			return NULL;
		}
		else{
			if(pid == -1){
				printf("\t\tAN ERROR OCCURED IN CREATION OF PROCESS\n");
			}
			else{
				printf("\tSTARTED WORKER WITH PID %d\n",pid);
				pool->processes[pool->process_count] = pid;
				pool->process_count++;
			}
		}
	}
	
		
        /* Done initializing; We can now return the queue. */
        pthread_mutex_unlock(&(pool->mutex));

	cleanup:

	free(new_name);
	printf("DONE INITIALIZING POOL (%d WORKERS CREATED)\n",pool->process_count);
        return pool;
}

void destroy_process_pool(ProcessPool* pool){
	printf("DESTROYING PROCESS POOL\n");
	for (int i = 0 ; i < pool->process_count ; i++){
		kill(pool->processes[i],SIGKILL);
	}
	destroy_queue(pool->parameter_queue);
	shm_unlink(pool->name);
	munmap(pool,PROCESS_POOL_SIZE);
	printf("DONE DESTROYING POOL\n");
}
void start_worker(ProcessPool* pool){
	struct timespec tm;
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


	exit(1);
}

