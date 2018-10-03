/**
* FRAMEWORK TO ENABLE MAP REDUCE
* Written by: Daniel Pattathil for CS416-F18
* Responsibilities: To set up a basic main() to enable unique map() reduce()
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char** argv){

     char* app = argv[2];
     char* impl = argv[4];
     int num_maps = atoi(argv[6]);
     int num_reduces = atoi(argv[8]);
     FILE* input_file = fopen(argv[10], "r" );

     const char* delimiters = " .,;:!-\n\t";

     char buffer[BUFFER_SIZE];
     char* last_token;

     if( input_file == NULL ){
        fprintf(stderr, "Unable to open file %s\n", argv[10]);
   }else{
        while( fgets(buffer, BUFFER_SIZE, input_file) != NULL ){
             last_token = strtok( buffer, delimiters );
             while( last_token != NULL ){
                  //change
                  printf( "%s\n", last_token );
                  last_token = strtok( NULL, delimiters );
             }
        }

        if( ferror(input_file) ){
             perror( "The following error occurred" );
        }

        fclose( input_file );
    }

    return 0;
}
