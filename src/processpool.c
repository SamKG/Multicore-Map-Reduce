/**
* PROCESS POOL LIBRARY
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: To allow an arbitrary number of worker processes to work on a common task
*/

#include <unistd.h>
#include <queue.h>
#include <pthread.h>
#include <sys/types.h>
#include <sharedmem.h>

#define MAX_POOL_SIZE 100 
#define MAX_POOL_NAME_SIZE 256

typedef struct process_pool{
	int process_count;
	Queue* parameter_queue;
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutex_attr;
	char name[MAX_POOL_NAME_SIZE];
} ProcessPool;

const int PROCESS_POOL_SIZE = sizeof(ProcessPool);


/**
* Returns a pointer to a new ProcessPool, allocated in shared memory space. If it already exists, returns that instead.
*
*Inputs:
*	name	-	The name to assign to the process pool
*	
*/
ProcessPool* new_process_pool(char* name, int number_workers){
	char* new_name = append_string(name,"_POOL");
        int exists_flag = 0;
	int shared_fd = get_shared_fd(new_name,PROCESS_POOL_SIZE,&exists_flag);

	/* Initialize space for process pool */
	ProcessPool* pool = (ProcessPool*) mmap(NULL, PROCESS_POOL_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_fd, 0);
	
	/* If it already exists, we don't need to reinitialize stuff */
	if (exists_flag){
		goto cleanup;
	}

	pthread_mutexattr_init(&(pool->mutex_attr));
        pthread_mutexattr_setpshared(&(pool->mutex_attr), PTHREAD_PROCESS_SHARED);

        /* Initialize mutex (note - we need to set shared memory attr in mutex) */
        pthread_mutex_init(&(pool->mutex), &(pool->mutex_attr));

        /* Lock the mutex while we initialize the queue */
        pthread_mutex_lock(&(pool->mutex));

        strcpy(pool->name,new_name);
	pool->queue = new_queue(name);
	
        /* Done initializing; We can now return the queue. */
        pthread_mutex_unlock(&(pool->mutex));

	cleanup:

	free(new_name);
        return pool;
}
