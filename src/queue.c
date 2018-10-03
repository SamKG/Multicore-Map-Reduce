/**
* SHARED MEMORY QUEUE STRUCTURE
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: To allow for a queue data structure to exist cross processes
*/




#include <pthread.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sharedmem.h>
/* CONSTANTS */
#define MAX_QUEUE_SIZE 100
#define MAX_QUEUE_NAME_SIZE 256


/* STRUCT DEFINITIONS */
/**
* Node used to store data (eg: function pointers, function args, etc...)
* 
*/
typedef struct Node{
	void* data;
} Node;

/**
* Threadsafe queue used to allow shared queue access
* 
*/
typedef struct Queue{
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutex_attr;
	int count;
	int front_pos;
	int back_pos;
	Node array[MAX_QUEUE_SIZE];
	char name[MAX_QUEUE_NAME_SIZE];			
} Queue;



const int QUEUE_SIZE_T = sizeof(Queue);
const char* QUEUE_NAME_MODIFIER = "_QUEUE"
/**
* Returns a pointer to a queue, present in a shared memory space. If the queue already exists, returns a pointer to it.
*
* inputs:
* char* name	-	The name to assign to the shared memory space of the queue
*/
Queue* new_queue(char* name){	
	char* new_name = append_string(name,"_QUEUE");
	int exists_flag = 0;
	int shared_fd = get_shared_fd(new_name,QUEUE_SIZE_T,&exists_flag);

	/* Create the memory mapped region */
	Queue* queue = (Queue*) mmap(NULL, QUEUE_SIZE_T, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,shared_fd,0);
	
	/* We don't need to reinitialize stuff, cause it already exists */
	if (exists_flag){
		goto cleanup;	
	}

	/* Initialize attributes for mutex */
	pthread_mutexattr_init(&(queue->mutex_attr));
	pthread_mutexattr_setpshared(&(queue->mutex_attr), PTHREAD_PROCESS_SHARED);

	/* Initialize mutex (note - we need to set shared memory attr in mutex) */
	pthread_mutex_init(&(queue->mutex), &(queue->mutex_attr));

	/* Lock the mutex while we initialize the queue */
	pthread_mutex_lock(&(queue->mutex));
	
	queue->count = 0;
	queue->front_pos = 0;
	queue->back_pos = 0;
	
	strcpy(queue->name,new_name);
	
	/* Done initializing; We can now return the queue. */
	pthread_mutex_unlock(&(queue->mutex));

	cleanup:

	free(new_name);
	return queue;
}


void queue_insert(Queue* queue, void* data, size_t data_size){
			
}	
