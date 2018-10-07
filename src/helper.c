#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printFile(char *outfile, key_value *result, char *app){
	FILE *fp;
	int i, j;
	void *ptr = general_shm_ptr;
	char *path = malloc(sizeof(char)*31);
	//^assumes that file name wont be more than 30 chars long
	sprintf(path, "/%s", outfile);
	fp = fopen(path, "w");
	free(path);
	char *key;
	if(strcmp(app, "wordcount") == 0){
		//print key/tvalue
		//need to decide on max
		for(i=0; i<MAXVALUE; i++){
			key = (char *) ptr + result[i]->key_offset;
			fprintf(fp, "%s\t%d", key, result[i]->value);
			if(result[i+1]->key_offset == NULL){
				//can i compare to null? is this error
				fclose(fp);
				return;
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
		return;
	}else{
		//must be sort since already checked in beginning
		//print key/n value# of times
		for(i=0; i<MAXVALUE; i++){
			key = (char *) ptr + result[i]->key_offset;
			for(j=0; j<result[i]->value; j++){
				fprintf(fp, "%s\n", key);
			}
			if(result[i+1]->key_offset == NULL){
				fclose(fp);
				return;
		}
		fclose(fp);
		return;
	}
}

void sort(int offset, int totalKeys, char *app){
	void *ptr = general_shm_ptr;
	int keyvalsz = sizeof(key_value);
	int i, j;
	key_value *next;
	key_value *prev;
	char *nextKey, *prevKey;
	key_value *tempInfo = malloc(sizeof(key_value));
	for(i = 1; i< totalKeys; i++){
		j=i;
		while(j > 0){
			next = (key_value *) ptr + offset + (j*keyvalsz);
			prev = (key_value *) ptr + offset + ((j-1)*keyvalsz);
			nextKey = (char *) ptr + next->key_offset;
			prevKey = (char *) ptr + prev->key_offset;
			if(strcmp(app, "wordcount") == 0){
				if(strcmp(nextKey, prevKey) > 0){
					break;
				}
			}else {//guaranteed to be int sort compare by ints
				if(atoi(nextKey) > atoi(prevKey)){
					break;
				}
			}
			*tempInfo->key_offset = next->key_offset;
			*tempInfo->value = next->value;
			*next->key_offset = prev->key_offset;
			*next->value = prev->value;
			*prev->key_offset = tempInfo->key_offset;
			*prev->value = tempInfo->value;
			j--;
		}
	}
	return;
}	







