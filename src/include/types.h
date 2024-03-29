#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_QUEUE_SIZE 100000
#define MAX_QUEUE_NAME_SIZE 256
#define MAX_POOL_SIZE 100 
#define MAX_POOL_NAME_SIZE 256
#define KEY_SIZE 1024
#define ValueType long long

typedef enum impl{WORDCOUNT,SORT,USER} App;
typedef enum op{Map,Reduce,Error} Operation;
typedef enum datatype{LONG, STRING, DOUBLE} DataType;
/* STRUCT DEFINITIONS */
extern App app_type;
typedef struct datachunk{
	int size;
	DataType data_type;
	int data;
} DataChunk;	

/**
* Node used to store data (eg: function pointers, function args, etc...)
* NOTE: data_offset is NOT a pointer! data is an OFFSET for the general_shm_ptr pointer! (this is due to mremap compatibility reasons) 
*/
typedef struct Node{
	Operation operation;
	int num_chunks;
	int data_offset;
	int meta;
} Node;

/**
* Threadsafe queue used to allow shared queue access
* 
*/
typedef struct Queue{
        pthread_mutex_t mutex;
        pthread_mutexattr_t mutex_attr;
        sem_t semaphore_lock;
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
	int return_array_offset;
	int return_array_count;
	int num_running_workers;
	int running;
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
        int thread_count;
	int return_array_offset;
	int return_array_count;
	int num_running_workers;
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
	int key_offset;
	ValueType value;
} KeyValue;

#endif
