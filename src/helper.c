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
int eq(DataChunk a, DataChunk b){
	if (a.data_type == b.data_type){
		char* da =(char*)( general_shm_ptr + a.data);
		char* db = (char*) ( general_shm_ptr + b.data);
		switch (a.data_type){
			case LONG:;
				long la =strtol(da,NULL,10);
				long lb = strtol(db,NULL,10);
				return la == lb;
				break;	
			case STRING:;
				return (strcmp(da,db) == 0); 
				break;
		}
	}
	return 0;
}

lt_key(KeyValue* v1, KeyValue* v2){
	DataChunk* a = (DataChunk*) (general_shm_ptr + v1->key_offset);
	DataChunk* b = (DataChunk*) (general_shm_ptr + v2->key_offset);
	if (a->data_type == b->data_type){
		char* da =(char*)( general_shm_ptr + a->data);
		char* db = (char*) ( general_shm_ptr + b->data);
		switch (a->data_type){
			case LONG:;
				long la =strtol(da,NULL,10);
				long lb = strtol(db,NULL,10);
				if( la < lb){
					return -1;
				}
				else if (la == lb){
					return 0;
				}
				return 1;
				break;	
			case STRING:;
				return strcmp(da,db); 
				break;
		}
	}
	return -1;
}
void sort(int offset, int totalKeys){
	KeyValue* arr = (KeyValue*)(general_shm_ptr + offset);
	qsort(arr,totalKeys,sizeof(KeyValue),&lt_key);
}	







