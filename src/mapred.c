/**
 * FRAMEWORK TO ENABLE MAP REDUCE
 * Written by: Daniel Pattathil for CS416-F18
 * Responsibilities: To set up a basic main() to enable unique map() reduce()
 */

#include <queue.h>
#include <processpool.h>
#include <threadpool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <sharedmem.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

#define BUFFER_SIZE 1024

typedef enum impl{THREAD,PROCESS} Implementation;
int main(int argc, char** argv){

	//Parse commandline arguments
	char* app = argv[2];
	char* impl = argv[4];
	int num_maps = atoi(argv[6]);
	int num_reduces = atoi(argv[8]);
	FILE* input_file = fopen(argv[10], "r" );
	FILE* output_file = fopen(argv[12],"rw");
	
	
	Implementation impl_type = THREAD;	
	// Check whether we are using processes or threads
	if (strcmp(impl,"procs") == 0){
		impl_type = PROCESS;	 
	}
	//Setup pools
	
	//unlink existing pools	
	shm_unlink("MAPRED");
        shm_unlink("MAPRED_MAP_QUEUE");
        shm_unlink("MAPRED_MAP_POOL");
        shm_unlink("MAPRED_MAP_TPOOL");
	
        shm_unlink("MAPRED_REDUCE_QUEUE");
        shm_unlink("MAPRED_REDUCE_POOL");
        shm_unlink("MAPRED_REDUCE_TPOOL");
        
	shm_unlink("GENERAL_SHM");
	
	//Declare all pools
	ProcessPool* map_pool_p = NULL;
	ThreadPool* map_pool_t = NULL;
		
	ProcessPool* reduce_pool_p = NULL;
	ThreadPool* reduce_pool_t = NULL;
	
	//Initialize pools based on implementation
	switch(impl_type){
		case THREAD:
			map_pool_t = new_thread_pool("MAPRED_MAP",num_maps);
			reduce_pool_t = new_thread_pool("MAPRED_REDUCE",num_reduces);
			break;
		
		case PROCESS:
			map_pool_p = new_process_pool("MAPRED_MAP",num_maps);
			reduce_pool_p = new_process_pool("MAPRED_REDUCE",num_reduces);
			break;
		default:
			printf("ERROR: NO IMPL SPECIFIED\n");
			return 0;
	}
	
	//1) Parse the input file

	//2) Chunk the parsed info
	
	//3) Pass chunks to map pool
	
	//4) Combine data passed back from map pool (ie: sort by key)

	//5) Pass all of one key to reduce pool

	//6) Retrieve output data, and output to file

	
	cleanup:
	//define cleanup stuff here
	if (map_pool_p != NULL){
		destroy_process_pool(map_pool_p);
	}	
	if (map_pool_t != NULL){
		destroy_thread_pool(map_pool_t);
	}	
	if (reduce_pool_p != NULL){
		destroy_process_pool(reduce_pool_p);
	}	
	if (reduce_pool_t != NULL){
		destroy_thread_pool(reduce_pool_t);
	}	
	shm_destroy_general();
	return 0;
}
