#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 10000
#define MAX_QUEUE_NAME_SIZE 256
#define MAX_POOL_SIZE 100 
#define MAX_POOL_NAME_SIZE 256
#define KEY_SIZE 1024
#define ValueType long long


typedef enum op{Map,Reduce} Operation;
typedef enum datatype{LONG, STRING, DOUBLE} DataType;
/* STRUCT DEFINITIONS */

typedef struct datachunk{
	int size;
	DataType data_type;
	void* data
} DataChunk;	

/**
* Node used to store data (eg: function pointers, function args, etc...)
* NOTE: data_offset is NOT a pointer! data is an OFFSET for the general_shm_ptr pointer! (this is due to mremap compatibility reasons) 
*/
typedef struct Node{
	Operation operation;
	int num_chunks;
	int data_offset;
} Node;

/**
* Threadsafe queue used to allow shared queue access
* 
*/
typedef struct Queue{
        pthread_mutex_t mutex;
        pthread_mutexattr_t mutex_attr;
        pthread_cond_t condition_changed;
        pthread_condattr_t condition_attr;
        int count;
        int front_pos,rear_pos;
	Node array[MAX_QUEUE_SIZE];
        char name[MAX_QUEUE_NAME_SIZE];
} Queue;

/**
* Threadsafe process pool used to allow management of process pool
*/
typedef struct process_pool{
        int process_count;
        Queue* parameter_queue;
        pthread_mutex_t mutex;
        pthread_mutexattr_t mutex_attr;
	int processes[MAX_POOL_SIZE];
        char name[MAX_POOL_NAME_SIZE];
} ProcessPool;

/**
* Threadsafe thread pool used to allow management of process pool
*/
typedef struct thread_pool{
        int thread_count;;
	int running;
        Queue* parameter_queue;
        pthread_mutex_t mutex;
        pthread_mutexattr_t mutex_attr;
	pthread_t threads[MAX_POOL_SIZE];
        char name[MAX_POOL_NAME_SIZE];
} ThreadPool;
/**
* Key-Value pair for usage in being passed around
*/

typedef struct key_value{
	char* key;
	ValueType value;
} KeyValue;

#endif
