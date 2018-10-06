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

int main(void){
	printf("START\n");
	shm_unlink("TEST3");
	shm_unlink("TEST3_QUEUE");	
	shm_unlink("TEST3_POOL");	
	shm_unlink("GENERAL_SHM");	
	shm_unlink("TEST3_TPOOL");

	struct sysinfo inf;
	sysinfo(&inf);
	printf("%lu Free memory on system\n",inf.freeram);
	
	unsigned long freeram = inf.freeram;
	int planned_ram = 1<<30;
	printf("%d Planned allocation of ram\n",planned_ram);
	while(planned_ram > freeram){
		planned_ram/=2;
	}
	// NOTE: We might have to make this shm general pool reaaaaally big, cause resize doesn't work for some reason :/
	shm_init_general(planned_ram);
	ThreadPool* pool = new_thread_pool("TEST3",1);
	for (int i = 0 ; i < 10; i++){
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
