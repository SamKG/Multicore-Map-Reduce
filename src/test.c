#include <queue.h>
#include <processpool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <sharedmem.h>
int main(void){
	printf("START\n");
	shm_init_general(1<<20);	
	ProcessPool* pool = new_process_pool("TEST3",10);
	for (int i = 0 ; i < 500; i++){
		Node tmp;
		tmp.data = shm_get_general(256);
		sprintf((char*)(general_shm_ptr+tmp.data),"test %d",i);
		queue_enqueue(pool->parameter_queue,tmp);
	}	
	sleep(1);
	destroy_process_pool(pool);
	printf("END\n");	
}
