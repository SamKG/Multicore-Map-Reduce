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

int main(void){
	int fd = open("/dev/cryptctl",O_NONBLOCK);	
	printf("OPENED WITH FD %d\n",fd);
	printf("ERR %s\n",strerror(errno));
	int ret = ioctl(fd,1);
	printf("RECEIVED ENCRYPT/DECRYPT PAIR %d\n",ret);
	if (ret >= 0){
		char* tmp[100];
		sprintf(tmp,"/dev/encrypt%d",ret);
		printf("OPENING %s\n",tmp);
		cryptctl_arg args;
		args.workerNum = ret;
		args.cipher = "POTATO";
		args.cipherLength = 6;
		int encr = open(tmp,O_RDWR);
		
		ioctl(fd,3,&args);
		int ret;	
	int x = write(encr,"tomato",6,&ret);
		printf("Wrote %d bytes (return %d)\n",ret,x);
		read(encr,tmp,6);
		tmp[6] = '\0';
		printf("Read %s \n",tmp);
	}
	close(fd);
	printf("DONE WITH CONTROLLER!\n");
}
