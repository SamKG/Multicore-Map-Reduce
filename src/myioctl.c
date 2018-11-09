#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct pair{
	int fd1;
	int fd2;
}pair;

int ioctl_create(int fd, char *key){
	int a = ioctl(fd, CRYPTCTL_CREATE, key);
	if(a == -1){
		printf("create pair failed");
	}
	return a;
}


void ioctl_delete(int fd1, int fd2 ){

}

void ioctl_change_key(int fd1, int fd2, char *key){
	int a = ioctl(fd1, CHANGE_KEY, key);
	int b = ioctl(fd2, CHANGE_KEY, key);
	if(a < 0|| b < 0){
		printf("Error changing key\n");
	}else{
		printf("Change key successful\n");
	}
	return;		


} 
void ioctl_encrypt(int fd, char *plain){
	int a = write(fd, plain, strlen(plain));
	if(a < 0){
		printf("Error writing to device\n");
	}else{
		printf("Write to device successful\n");
		printf("Press enter to read encrypted text\n");
		getchar();
		char message[MAX_MESSAGE]; //need to define max 
		int b = read(fd, message, MAX_MESSAGE);
		if(b < 0){
			printf("Error reading from device\n");
		}else{
			printf("Read from device successful\n");
		}
	}
	return; 
		
}

void ioctl_decrypt(int fd, char *ciphtext){
	int a = write(fd, ciphtext, strlen(ciphtext));
	if(a < 0){
		printf("Error writing to device\n");
	}else{
		printf("Write to device successful\n");
		printf("Press enter to read encrypted text\n");
		getchar();
		char message[MAX_MESSAGE]; //need to define max 
		int b = read(fd, message, MAX_MESSAGE);
		if(b < 0){
			printf("Error reading from device\n");
		}else{
			printf("Read from device successful\n");
		}
	}
	return;
}

int main(int argc, char** argv){
	//how much we should allocate for one line of input MAX MESSAGe
	char *ctl = "/dev/cryptctl";
	int ctlfd = open(ctl, O_RDWR);
	if(ctlfd == -1){
		printf("cryptctl open already");
	}
	char input[MAX_MESSAGE];
	char *token;
	pair devices[100];
	char *enc = "cryptEncrypt";
	char *dec = "cryptDecrypt";
	//set max number of devices capable to 100
	int i=1;
//not sure how we want this program to end
	while(i ==1){
		fgets(input, 10000, stdin);
		token = strtok(input, " ");
		if(strcmp(token, "create") == 0){
			token = strtok(input, " "); 
			int index = ioctl_create(ctlfd, token);
			char *edev = malloc(16);
			char *ddev = malloc(16);
			char *ind;
			sprintf(ind, "%d", index);
			strcpy(edev, enc);
			strcat(edev, ind);
			strcpy(ddev, dec);
			strcat(ddev, ind);
			devices[index].fd1 = open(edev, O_RDWR);
			devices[index].fd2 = open(ddev, O_RDWR);
			printf("Device pair %d was created with key %s\n", index, token);
			continue;
		}else if(strcmp(token, "delete") == 0){
			token = strtok(input, " ");
			int index = atoi(token);
			ioctl_delete(devices[index].fd1, devices[index].fd2);
			printf("Device pair %d was destroyed\n", index);
			continue;
		}else if(strcmp(token, "change_key") == 0){
			token = strtok(input, " ");
			char *ind;
			ind = strtok(input, " ");
			int index = atoi(ind);
			ioctl_change_key(devices[index].fd1, devices[index].fd2, token);
			printf("Device pair %d's key was changed to %s\n",index, token);
			continue;
		}else if(strcmp(token, "encrypt") == 0){
			token = strtok(input, " ");
			int index = atoi(token);
			char *text; //do i have to malloc this
			text = strtok(input, " ");
			ioctl_encrypt(devices[index].fd1, text);
			continue;
		}else if(strcmp(token, "decrypt") == 0){
			token = strtok(input, " ");
			int index = atoi(token);
			char *text;
			text = strtok(input, " ");
			ioctl_decrypt(devices[index].fd2, text);
			continue;

		}else {
			printf("Incorrect input syntax. Please enter one of the following: 'create <key>', 'delete <index>', 'change_key <newKey> <index>', 'encrypt <index> <plainText>', or 'decrypt <index> <cipherText>'\n");
			continue;
		}
	}
	return 0;
}

