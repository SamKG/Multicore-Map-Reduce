/**
* SHARED MEMORY HANDLER 
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: Handling all shm_open calls
*/



/**
* Gets a fd for the shared memory, given a name and size. If none exists, it creates it first.
*/
int get_shared_fd(char* name, int size){
	int shared_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0666);

        /* Incase of error */
        while (shared_fd == -1){
                switch(errno){
                        case EEXIST: /* Already exists; We have to simply mmap and return region */
                                shared_fd = shm_open(name, O_RDWR, 0666);
                                return shared_fd;
                        default:
                                return -1;
                                break;
                }
        }

        /* Set shared memory size */
        ftruncate(shared_fd,size);
	
	return shared_fd;
}
