/**
 * FRAMEWORK TO ENABLE MAP REDUCE
 * Written by: Daniel Pattathil for CS416-F18
 * Responsibilities: To set up a basic main() to enable unique map() reduce()
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <sharedmem.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <filehandler.h>

#define BUFFER_SIZE 1024
int tokenize_file(FILE* file,int* num_chunk_count){
	const char* delimiters = " .,;:!-\n\t";

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
				//insert token into a shm_general region
				int token_len = strlen(last_token)+1;
				printf("ALLOC SPACE FOR STR %s\n",last_token);
				int str_offset = shm_get_general(token_len);
				strcpy((char*) (general_shm_ptr + str_offset),last_token);
				
				//make a datachunk to store
				arr[num_chunks].size = token_len;
				arr[num_chunks].data_type = STRING;
				arr[num_chunks].data = str_offset;	
				num_chunks++;

				last_token = strtok( NULL, delimiters );
			}
		}
		printf("\tFILE TOKENIZER COPYING DATA CHUNKS TO SHM\n");
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
