/**
 * FRAMEWORK TO ENABLE MAP REDUCE
 * Written by: Daniel Pattathil for CS416-F18
 * Responsibilities: To set up a basic main() to enable unique map() reduce()
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <types.h>
#include <sharedmem.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <filehandler.h>
#include <mapred_def.h>

#define BUFFER_SIZE 1024
int tokenize_file(FILE* file,int* num_chunk_count){
	const char* delimiters = " .,;:!-\r\n\t";

	char buffer[BUFFER_SIZE];
	char* last_token;




	int num_chunks = 0;
	DataChunk arr[100000];
	if( file== NULL ){
		fprintf(stderr, "Unable to open file \n");
	}
	else{
		
		while( fgets(buffer, BUFFER_SIZE, file) != NULL ){
			last_token = strtok( buffer, delimiters );
			while( last_token != NULL ){
				for (int i = 0 ; last_token[i] != '\0' ; i++){
					last_token[i] = tolower(last_token[i]);
				}
				//insert token into a shm_general region
				int token_len = strlen(last_token)+1;
				//printf("ALLOC SPACE FOR STR %s (len: %d)\n",last_token,token_len);
				int str_offset = shm_get_general(token_len);
				strcpy((char*) (general_shm_ptr + str_offset),last_token);
				
				//make a datachunk to store
				arr[num_chunks].size = token_len;
				arr[num_chunks].data_type = (app_type == SORT)?LONG:STRING;
				arr[num_chunks].data = str_offset;	
				//printf("\tSTR OFFSET STRING %s\n",(char*)(general_shm_ptr + str_offset));
				num_chunks++;

				last_token = strtok( NULL, delimiters );
			}
		}
		printf("\tFILE TOKENIZER COPYING DATA CHUNKS TO SHM (counted %d words)\n",num_chunks);
		int datachunk_offset = shm_get_general(sizeof(DataChunk)*num_chunks);
		for (int i = 0 ; i<num_chunks; i++){
			DataChunk* ptr = (DataChunk*) (general_shm_ptr + datachunk_offset + (i*sizeof(DataChunk)));
			*ptr = arr[i];
		}	
		*num_chunk_count = num_chunks;
		return datachunk_offset;
		if( ferror(file) ){
			perror( "The following error occurred" );
		}

	}

	return -1;
}
void output_file(FILE* file,int offset, int count){
	for (int i = 0 ; i < count ; i++){
                DataChunk* dc = (DataChunk*) (general_shm_ptr + offset + i*sizeof(DataChunk));

                fprintf(file,"%s\n",(char*)(general_shm_ptr +  dc->data));
        }
}
