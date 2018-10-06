#include <queue.h>
#include <processpool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
int main(void){
	printf("START\n");
	
	ProcessPool* pool = new_process_pool("TEST3",10);
	for (int i = 0 ; i < 500; i++){
		Node tmp;
		sprintf(tmp.data,"test %d",i);
		queue_enqueue(pool->parameter_queue,tmp);
	}	
	sleep(1);
	destroy_process_pool(pool);
	printf("END\n");	
}
