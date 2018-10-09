/*:\
  Daniel Pattathil - CS 416
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sharedmem.h>
#include <types.h>
#include <mapred_def.h>
#include <helper.h>

KeyValue* map(int offset, int size, int* countreturn){
	//printf("RUNNING MAP\n");
	int i, j;
	void* ptr = general_shm_ptr;
	int chunksz = sizeof(DataChunk);
	int keyvalsz = sizeof(KeyValue);
	KeyValue temp_kv;
	KeyValue keyvalues[size];

	//This is creating an array of KeyValue pairs with offset values to chunks
	for(i = 0; i < size; i++)
	{
		keyvalues[i].key_offset = offset + (i*chunksz);
		keyvalues[i].value = 1;
	}

	//This is to sort the array of KeyValue pairs
	DataChunk* next_chunk;
	DataChunk* prev_chunk;
	
	//printf("\tSORTING KEYS\n");
	for (i = 1 ; i < size; i++) {
		j = i;
		while (j > 0){
			next_chunk = (DataChunk *) (ptr + keyvalues[j].key_offset);
			prev_chunk = (DataChunk *) (ptr + keyvalues[j-1].key_offset);

			//sort these tokens in order
			if(gt(*prev_chunk, *next_chunk))
			{
				break;
			}
			temp_kv = keyvalues[j];
			keyvalues[j] = keyvalues[j-1];
			keyvalues[j-1] = temp_kv;
			j--;
		}
	}

	//this compresses the array if there are same values within this chunk
	//printf("\tCOMPRESSING ARRAY\n");
	int index;
	int count = 0;
	
	KeyValue final_arr[size];
	final_arr[0] = keyvalues[0];
	int curr_pos = 1;
	for(i = 1; i < size; i++){
		KeyValue comp = final_arr[curr_pos-1];	
		KeyValue curr = keyvalues[i];
		DataChunk* first = (DataChunk*) (ptr + comp.key_offset); 
		DataChunk* second = (DataChunk*) (ptr + curr.key_offset);
		if (eq(*first,*second)) {
			final_arr[curr_pos-1].value+=curr.value;
		}
		else {
			final_arr[curr_pos++] = curr;
		}
	}
	
	KeyValue* return_arr = (KeyValue*) malloc(sizeof(KeyValue)*curr_pos);
	for(int i = 0 ; i < curr_pos ; i++){
		//printf("COPYING POS %d\n",i);
		return_arr[i] = final_arr[i];
		//printf("\t%d\n",return_arr[i].value);
	}
	*countreturn = curr_pos;
	//printf("\tDONE WITH MAP\n");
	//This is the offset for the resulting array of mappped KeyValue pairs
	return return_arr;
}

char* reduce(int start, int count){
	//printf("\tSTART REDUCE\n");
	long val = 0;
	char* key = NULL;
	for (int i = 0 ; i < count ; i++){
		KeyValue* kv = (KeyValue*) (general_shm_ptr + start + (i*sizeof(KeyValue)));
		val+=kv->value;
		if (key == NULL){
			DataChunk* k = (DataChunk*) (general_shm_ptr + kv->key_offset);
			key = (char*) (general_shm_ptr+k->data);
		}
	}	
	
	char* final_str = (char*) malloc(strlen(key) + 40);
	switch (app_type){
		case WORDCOUNT:;
			sprintf(final_str,"%s\t%lu",key,val);	
			return final_str;
			break;
		case SORT:;
			sprintf(final_str,"%s",key,val);	
			return final_str;
			
	}	
}
