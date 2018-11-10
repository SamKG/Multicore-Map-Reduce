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
	int fd = open("/dev/cryptctl",0);	
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
		sprintf(tmp,"/dev/decrypt%d",ret);
		int decr = open(tmp,O_RDWR);
		
		ioctl(fd,3,&args);
		int ret;	
		int x = write(encr,"tomato",6,&ret);
		char* tmp2[100];
		read(encr,tmp2,6);
		tmp2[6] = '\0';
		tmp2[7] = '\0';
		fflush(stdout);
		printf("Read %s \n",tmp2);

		int y = write(decr,tmp2,6,&ret);
		read(decr,tmp2,6);
		tmp2[6] = '\0';
		tmp2[7] = '\0';
		fflush(stdout);
		printf("Read %s \n",tmp2);
	}
	close(fd);
	printf("DONE WITH CONTROLLER!\n");
}
