#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
typedef struct cryptctl_arg{
        int workerNum;
        char* cipher;
        int cipherLength;
} cryptctl_arg;

int main(int argc, char** argv){
	
	int command = 0;
	char* cipher;
	int fd,ret,id;
	if (argc > 1){
		if (argv[1][1] == 'c'){
			command = 1;
		}	
		else if (argv[1][1] == 's' && argc > 3){
			command = 2;
			id = atoi(argv[2]);
			cipher = argv[3];
		}
	}
	switch(command){
	case 1:;
        	fd = open("/dev/cryptctl",0);
		ret = ioctl(fd,1);
		printf("Created new pair with id %d\n",ret);
		close(fd);
		break;
	case 2:;
        	fd = open("/dev/cryptctl",0);
		ret = ioctl(fd,1);
                cryptctl_arg args;
                args.workerNum = id;
                args.cipher = cipher;
                args.cipherLength = strlen(cipher);

		ioctl(fd,3,&args);
		printf("Changed the cipher of %d to %s\n",id,cipher);
		close(fd);
		break;
	default:;
		printf("Invalid usage! Please use as either:\n\tcryptdriver -c (create a new encrypt/decrypt pair)\n\tcryptdriver -s <pair id> <cipher> (to change the cipher for a given pair)\n");  
	}
	return 0;
}
