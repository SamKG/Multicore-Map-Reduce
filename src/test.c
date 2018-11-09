#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
int main(void){
	int fd = open("/dev/cryptctl",O_NONBLOCK);	
	printf("OPENED WITH FD %d\n",fd);
	printf("ERR %s\n",strerror(errno));
	int ret = ioctl(fd,1);
	close(fd);
	printf("DONE!\n");
}
