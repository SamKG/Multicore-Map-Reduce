/**
* SHARED MEMORY QUEUE STRUCTURE
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: To allow for a queue data structure to exist cross processes
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sharedmem.h>
#include <types.h>
#include <queue.h>
/* CONSTANTS */
const int QUEUE_SIZE_T = sizeof(Queue);
const char* QUEUE_NAME_MODIFIER = "_QUEUE";
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

	pthread_condattr_init(&(queue->condition_attr));	
	pthread_condattr_setpshared(&(queue->condition_attr),PTHREAD_PROCESS_SHARED);
	pthread_cond_init(&(queue->condition_changed),&(queue->condition_attr));
		
	queue->count = 0;
	queue->front_pos = 0; 
	queue->rear_pos = MAX_QUEUE_SIZE - 1;

	strcpy(queue->name,new_name);
	
	/* Done initializing; We can now return the queue. */
	pthread_mutex_unlock(&(queue->mutex));

	cleanup:

	free(new_name);
	return queue;
}

void destroy_queue(Queue* queue){
	shm_unlink(queue->name);
	munmap(queue,QUEUE_SIZE_T);;
}

int queue_is_empty(Queue* queue){
	return queue->count == 0;
}

int queue_is_full(Queue* queue){
	return queue->count == MAX_QUEUE_SIZE - 1;
}

Node queue_dequeue_private(Queue* queue){
	Node return_value;
	if (queue_is_empty(queue)){
		goto cleanup;
	}
	return_value = queue->array[queue->front_pos];
	queue->front_pos = (queue->front_pos + 1) % MAX_QUEUE_SIZE;
	queue->count--;	
	
	cleanup:
	return return_value;
}

Node queue_dequeue(Queue* queue){
	Node return_value;
	
	pthread_mutex_lock(&(queue->mutex));
	
	return_value = queue_dequeue_private(queue);

	pthread_cond_signal(&(queue->condition_changed));	
	pthread_mutex_unlock(&(queue->mutex));
	return return_value;
}

void queue_enqueue_private(Queue* queue,Node data){
	if (queue_is_full(queue)){
		goto cleanup;
	}
	
	queue->rear_pos  = (queue->rear_pos + 1) % MAX_QUEUE_SIZE;
	queue->array[queue->rear_pos] = data;
	queue->count++;	
	
	cleanup:
	return;
}
void queue_enqueue(Queue* queue, Node data){	
	pthread_mutex_lock(&(queue->mutex));

	queue_enqueue_private(queue,data);

	pthread_cond_signal(&(queue->condition_changed));	
	pthread_mutex_unlock(&(queue->mutex));
}



