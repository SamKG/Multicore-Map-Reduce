#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

#define MAX_QUEUE_SIZE 10000
#define MAX_QUEUE_NAME_SIZE 256
#define MAX_POOL_SIZE 100 
#define MAX_POOL_NAME_SIZE 256

/* STRUCT DEFINITIONS */
/**
* Node used to store data (eg: function pointers, function args, etc...)
* 
*/
typedef struct Node{
        char data[256];
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
#endif
