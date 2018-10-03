/**
* SHARED MEMORY HANDLER 
* Written by: Samyak K. Gupta for CS416-F18
* Responsibilities: Handling all shm_open calls
*/



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
