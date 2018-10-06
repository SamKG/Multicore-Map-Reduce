#include <processpool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void){
	printf("START\n");
	
	ProcessPool* pool = new_process_pool("TEST3",10);
	destroy_process_pool(pool);
	printf("END\n");	
}
