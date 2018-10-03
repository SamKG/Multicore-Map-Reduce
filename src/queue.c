#include <pthread.h>
#include <fcntl.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* CONSTANTS */
#define MAX_QUEUE_SIZE 100



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
} Queue;


const int QUEUE_SIZE_T = sizeof(Queue);

/**
* Returns a pointer to a queue, present in a shared memory space.
*
* inputs:
* char* name	-	The name to assign to the shared memory space of the queue
*/
Queue* new_queue(char* name){		
	int shared_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0666);
	ftruncate(shared_fd,QUEUE_SIZE_T);
	
	/* Create the memory mapped region */
	Queue* queue = (Queue*) mmap(NULL, QUEUE_SIZE_T, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,shared_fd,0);
	
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
	
	/* Done initializing; We can now return the queue. */
	pthread_mutex_unlock(&(queue->mutex));
	
	return queue;
}		
