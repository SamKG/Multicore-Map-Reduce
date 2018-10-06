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
int main(void){
	printf("START\n");
	shm_unlink("TEST3");
	shm_unlink("TEST3_QUEUE");	
	shm_unlink("TEST3_POOL");	
	shm_unlink("GENERAL_SHM");	
	shm_unlink("TEST3_TPOOL");

	// NOTE: We might have to make this shm general pool reaaaaally big, cause resize doesn't work for some reason :/
	shm_init_general(1<<30);	
	ThreadPool* pool = new_thread_pool("TEST3",10);
	for (int i = 0 ; i < 500; i++){
		Node tmp;
		tmp.data = shm_get_general(256);
		sprintf((char*)(general_shm_ptr+tmp.data),"test %d",i);
		queue_enqueue(pool->parameter_queue,tmp);
	}	
	sleep(1);
	destroy_thread_pool(pool);
	shm_destroy_general();
	printf("END\n");	
}
