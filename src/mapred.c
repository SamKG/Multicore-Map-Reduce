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
#include <filehandler.h>
#include <pthread.h>
#include <queue.h>
#include <helper.h>
#include <sys/sysinfo.h>
#define BUFFER_SIZE 1024
#define CHUNK_SIZE (num_chunks/num_maps)

typedef enum impl_type{THREAD,PROCESS} Implementation;
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
	
	struct sysinfo inf;
        sysinfo(&inf);
        printf("%lu Free memory on system\n",inf.freeram);

        unsigned long freeram = inf.freeram;
        int planned_ram = 1<<30;
        printf("%d Planned allocation of ram\n",planned_ram);
        while(planned_ram > freeram){
                planned_ram/=2;
        	sysinfo(&inf);
        }

	shm_init_general(1<<30);	
	printf("GENERAL SHM LOC: %lu\n",general_shm_ptr);
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
	printf("TOKENIZING FILE\n");
	int num_chunks = 0;
	int datachunk_offset = tokenize_file(input_file,&num_chunks);

	//2) Chunk the parsed info & pass to map workers
	printf("CHUNKING DATA\n");	
	//	i) Create shared memory region for return values
	int return_array_size = (num_chunks) + 1;
	int return_array_offset = shm_get_general(return_array_size * sizeof(KeyValue));
	
	//	ii) Set offsets of return in the pools;
	switch(impl_type){
		case THREAD:
			map_pool_t->return_array_offset = return_array_offset;
			map_pool_t->return_array_count = 0;
			break;
		case PROCESS:
			map_pool_p->return_array_offset = return_array_offset;
			map_pool_p->return_array_count = 0;
			break;
	}
	printf("\tSENDING INSTRUCTIONS TO POOL QUEUE\n");	
	//	iii) Send the instructions to the pool queue
	int remainder = num_chunks % CHUNK_SIZE;
	int total_num_splits =(remainder == 0)?num_chunks/CHUNK_SIZE:(num_chunks/CHUNK_SIZE + 1); 	
	for (int i = 0 ; i < total_num_splits; i++){
		Node new_instr;
		new_instr.operation = Map;
		new_instr.num_chunks = CHUNK_SIZE; 
		if(i == total_num_splits - 1 && remainder != 0){
			new_instr.num_chunks = remainder;
		}
		new_instr.data_offset = datachunk_offset + (i*sizeof(DataChunk)*CHUNK_SIZE);
		DataChunk* chnk = (DataChunk*)(general_shm_ptr + datachunk_offset + (i*sizeof(DataChunk)*CHUNK_SIZE));
		printf("\t\tCHUNK %d (numchunks: %d)\n",i,new_instr.num_chunks);
		switch(impl_type){
			case THREAD:
				queue_enqueue(map_pool_t->parameter_queue, new_instr);
				break;

			case PROCESS:
				queue_enqueue(map_pool_p->parameter_queue, new_instr);
				break;
			default:
				printf("ERROR: NO IMPL SPECIFIED\n");
				return 0;
		}
	}

	//	iv) Wait for the running threads to finish up with maps
	while(1){
		printf("WAITING FOR MAPS TO FINISH\n");
		switch(impl_type){
			case THREAD:
				pthread_mutex_lock(&(map_pool_t->parameter_queue->mutex));
				printf("\tQUEUE HAS %d ELEMENTS\n",map_pool_t->parameter_queue->count);
				if (queue_is_empty(map_pool_t->parameter_queue)){
					map_pool_t->running = 0;
					while(map_pool_t->num_running_workers > 0){
						printf("THERE ARE STILL %d WORKERS RUNNING\n",map_pool_t->num_running_workers);
						sleep(1);
					}
					goto exitmapwait;
				}
				pthread_mutex_unlock(&(map_pool_t->parameter_queue->mutex));
				break;
			case PROCESS:
				pthread_mutex_lock(&(map_pool_p->parameter_queue->mutex));
				printf("\tQUEUE HAS %d ELEMENTS\n",map_pool_p->parameter_queue->count);
				if (queue_is_empty(map_pool_p->parameter_queue)){
					map_pool_p->running = 0;
					while(map_pool_p->num_running_workers > 0){
						printf("THERE ARE STILL %d WORKERS RUNNING\n",map_pool_p->num_running_workers);
						sleep(1);
					}
					goto exitmapwait;
				}
				pthread_mutex_unlock(&(map_pool_p->parameter_queue->mutex));
				break;
		}
		sleep(1);
	}
	exitmapwait:
	printf("RECEIVED MAP DATA!\n");
	//Shuffle data in between
	switch(impl_type){
		case THREAD:
			sort(map_pool_t->return_array_offset,map_pool_t->return_array_count);	
			break;
		case PROCESS:
			sort(map_pool_p->return_array_offset,map_pool_p->return_array_count);	
			break;	
	}
	


	//3) Pass all of one key to reduce pool

	int reduce_array_size = (num_chunks) + 1;
	int reduce_array_offset = shm_get_general(reduce_array_size * sizeof(DataChunk));
	
	int num_input_pairs = 0;
	int map_return_offset = 0;	
	//	ii) Set offsets of return in the pools;
	switch(impl_type){
		case THREAD:
			num_input_pairs = map_pool_t->return_array_count;
			map_return_offset = map_pool_t->return_array_offset;
			reduce_pool_t->return_array_offset = reduce_array_offset;
			reduce_pool_t->return_array_count = 0;
			break;
		case PROCESS:
			num_input_pairs = map_pool_p->return_array_count;
			map_return_offset = map_pool_p->return_array_offset;
			reduce_pool_p->return_array_offset = reduce_array_offset;
			reduce_pool_p->return_array_count = 0;
			break;
	}
	printf("\tSENDING INSTRUCTIONS TO REDUCE POOL QUEUE\n");	
	//	iii) Send the instructions to the pool queue
	
	int count = 0;
	for (int i = 0 ; i < num_input_pairs; i++){
		Node curr_instr;
		curr_instr.operation = Reduce;
		curr_instr.num_chunks = 1;
		curr_instr.data_offset = map_return_offset + (i*sizeof(KeyValue));
		curr_instr.meta = reduce_array_offset + (count*sizeof(DataChunk));
		count+=1;
		KeyValue* curr_kv = (KeyValue*) (general_shm_ptr + curr_instr.data_offset);
		DataChunk* curr_dc = (DataChunk*) (general_shm_ptr + curr_kv->key_offset);
		char* curr_key = (char*) (general_shm_ptr + curr_dc->data);
	
		for (int j = i+1 ; j <= num_input_pairs ; j++){
			if(j==num_input_pairs){
				i = j;
				break;
			}
			KeyValue* next_kv = (KeyValue*) (general_shm_ptr + map_return_offset + (j*sizeof(KeyValue)));
			DataChunk* next_dc = (DataChunk*) (general_shm_ptr + next_kv->key_offset);
			char* next_key = (char*) (general_shm_ptr + next_dc->data);
			if(strcmp(next_key,curr_key) == 0){
				curr_instr.num_chunks++;
			}
			else{
				i = j-1;
				break;
			}
		}	
		printf("INSERT REDUCE OP FOR KEY %s count %lu\n",curr_key,curr_instr.num_chunks);
		switch(impl_type){
			case THREAD:
				queue_enqueue(reduce_pool_t->parameter_queue, curr_instr);
				break;

			case PROCESS:
				queue_enqueue(reduce_pool_p->parameter_queue, curr_instr);
				break;
			default:
				printf("ERROR: NO IMPL SPECIFIED\n");
				return 0;
		}
	}
	
	//now we wait for return from reduce pool
	
	while(1){
		printf("WAITING FOR REDUCES TO FINISH\n");
		switch(impl_type){
			case THREAD:
				pthread_mutex_lock(&(reduce_pool_t->parameter_queue->mutex));
				printf("\tQUEUE HAS %d ELEMENTS\n",reduce_pool_t->parameter_queue->count);
				if (queue_is_empty(reduce_pool_t->parameter_queue)){
					reduce_pool_t->running = 0;
					while(reduce_pool_t->num_running_workers > 0){
						printf("THERE ARE STILL %d WORKERS WORKING\n",reduce_pool_t->num_running_workers);
						sleep(1);
					}
					goto exitreducewait;
				}
				pthread_mutex_unlock(&(reduce_pool_t->parameter_queue->mutex));
				break;
			case PROCESS:
				pthread_mutex_lock(&(reduce_pool_p->parameter_queue->mutex));
				printf("\tQUEUE HAS %d ELEMENTS\n",reduce_pool_p->parameter_queue->count);
				if (queue_is_empty(reduce_pool_p->parameter_queue)){
					reduce_pool_p->running = 0;
					while(reduce_pool_p->num_running_workers > 0){
						printf("THERE ARE STILL %d WORKERS WORKING\n",reduce_pool_p->num_running_workers);
						sleep(1);
					}
					goto exitreducewait;
				}
				pthread_mutex_unlock(&(reduce_pool_p->parameter_queue->mutex));
				break;
		}
		sleep(1);
	}
	exitreducewait:
	printf("DONE WAITING FOR REDUCE\n");
	
	for (int i = 0 ; i < count ; i++){
		DataChunk* dc = (DataChunk*) (general_shm_ptr + reduce_array_offset + i*sizeof(DataChunk));
		
		printf("%s\n",(char*)(general_shm_ptr +  dc->data));		
	}	
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
