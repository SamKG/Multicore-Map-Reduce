/**
* PROCESS POOL LIBRARY
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: To allow an arbitrary number of worker processes to work on a common task
*/

#include <unistd.h>
#include <queue.h>
#include <sys/types.h>

#define MAX_POOL_SIZE 

typedef struct process_pool{
	int process_count;
	Queue* parameter_queue;
	char* name;n
}


