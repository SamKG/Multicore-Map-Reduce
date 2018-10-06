/**
* SHARED MEMORY HANDLER 
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: Handling all shm_open calls
*/
#define _GNU_SOURCE
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sharedmem.h>
#include <pthread.h>
#include <stdio.h>

/**
* Gets a fd for the shared memory, given a name and size. If none exists, it creates it first.
*/
int get_shared_fd(char* name, int size, int* exists_flag){
	int shared_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0666);

        /* Incase of error */
        while (shared_fd == -1){
                switch(errno){
                        case EEXIST: /* Already exists; We have to simply mmap and return region */
                                shared_fd = shm_open(name, O_RDWR, 0666);
				*exists_flag = 1;
                                return shared_fd;
                        default:
                                return -1;
                                break;
                }
        }

        /* Set shared memory size */
        ftruncate(shared_fd,size);
	
	*exists_flag = 0;
	return shared_fd;
}

/**
* Returns a concatenated string, allocated to a new memory region. *NOTE*: Caller is reponsible for freeing the string!
*/
char* append_string(char* one, char* two){
	char* newstr = (char*) malloc((1 + strlen(one) + strlen(two)) * sizeof(char));
	strcpy(newstr, one);
	strcat(newstr,two);
	return newstr;
}



/* GENERAL PURPOSE SHARED MEMORY SPACES */
int general_shm_pos = 0;
int general_shm_size = -1;
int general_shm_fd = 0;
void* general_shm_ptr = NULL;
pthread_mutex_t general_shm_mutex;
pthread_mutexattr_t general_shm_mutexattr;

void shm_init_general(int size){
	printf("INITIALIZING GENERAL_SHM\n");
	int exists_flag = 0;
	int shared_fd = get_shared_fd("GENERAL_SHM",size, &exists_flag);
	general_shm_fd = shared_fd;
	if (exists_flag){
		return;
	}
	pthread_mutexattr_init(&general_shm_mutexattr);
	pthread_mutexattr_setpshared(&general_shm_mutexattr, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(&general_shm_mutex, &general_shm_mutexattr);

	general_shm_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,shared_fd,0);
	general_shm_size = size;	
	printf("DONE\n");
}

void shm_resize_general(int size){
	printf("SHM_GENERAL: RESIZE SPACE TO %d\n",general_shm_size + (size*10));
	ftruncate(general_shm_fd,general_shm_size + (size*10));
	void* new_ptr = mremap(general_shm_ptr,general_shm_size,general_shm_size + (size*10),MREMAP_MAYMOVE,0);
	while(new_ptr == MAP_FAILED){
		new_ptr = mremap(general_shm_ptr,general_shm_size,general_shm_size + (size*10),MREMAP_MAYMOVE,0);
	}
	general_shm_ptr = new_ptr;
	general_shm_size += (size*10);
}

void shm_destroy_general(){
	munmap(general_shm_ptr,general_shm_size);
	shm_unlink("GENERAL_SHM");
}
/**
* Lol this is basically a lazy malloc function, written to use shared memory
*/
int shm_get_general(int size){
	pthread_mutex_lock(&general_shm_mutex);
	printf("SHM_GENERAL: ALLOC %d SPACE\n",size);
	if (general_shm_pos + size >= general_shm_size){
		/* have to resize shared space */
		shm_resize_general(size);
	}
	int data_offset = general_shm_pos;
	general_shm_pos += size;
	pthread_mutex_unlock(&general_shm_mutex);
	return data_offset;
}
