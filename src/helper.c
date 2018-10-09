#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <sharedmem.h>
#include <string.h>
#define MAXVALUE 100000000

void printFile(char *outfile, KeyValue *result, char *app){
}


int gt(DataChunk a, DataChunk b){
	if (a.data_type == b.data_type){
		char* da =(char*)( general_shm_ptr + a.data);
		char* db = (char*) ( general_shm_ptr + b.data);
		switch (a.data_type){
			case LONG:;
				long la =strtol(da,NULL,10);
				long lb = strtol(db,NULL,10);
				return la > lb;
				break;	
			case STRING:;
				return !(strcmp(da,db) < 0); 
				break;
		}
	}
	return 0;
}

void sort(int offset, int totalKeys){
	KeyValue* arr = (KeyValue*)(general_shm_ptr + offset);
	int i = 1;
	while ( i < totalKeys){
		int j = i;
		DataChunk* a = (DataChunk*) (general_shm_ptr + arr[j-1].key_offset);
		DataChunk* b = (DataChunk*) (general_shm_ptr + arr[j].key_offset);
		while (j > 0 && gt(*a,*b)){
			KeyValue tmp = arr[j];
			arr[j] = arr[j-1];
			arr[j-1] = tmp;
			j--;
			a = (DataChunk*) (general_shm_ptr + arr[j-1].key_offset);
			b = (DataChunk*) (general_shm_ptr + arr[j].key_offset);
		}
		i++;
	}
	return;
}	







