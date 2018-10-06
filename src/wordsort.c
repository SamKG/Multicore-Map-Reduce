#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

int main(int argc, char** argv){

}

int map(int offset, int size){
     int i, j;
     void* ptr = general_shm_ptr;
     int chunksz = sizeof(DataChunk);
     int keyvalsz = sizeof(key_value);
     key_value temp_kv;


     //This is the offset for the resulting array of mappped key_value pairs
     int mappedOffset = shm_get_general(size * keyvalsz);

     //This is creating an array of KeyValue pairs with offset values to chunks
     for(i = 0; i < size; i++)
     {
          temp_kv->key_offset = (offset + (i * chunksz));
          temp_kv->value = 1;
          *((key_value*) ptr + mappedOffset + (i * keyvalsz)) = temp_kv;
     }


     //This is to sort the array of KeyValue pairs
     DataChunk* left_chunk;
     DataChunk* right_chunk;
     key_value* left_key;
     key_value* right_key;

     for (i = 1 ; i < size; i++) {
          j = i;
          while (j > 0){
               left_key = ptr + mappedOffset + ((j-1) * keyvalsz);
               right_key = ptr + mappedOffset + (j * keyvalsz);
               left_chunk = left_key->key_offset + ptr;
               right_chunk = right_chunk->key_offset + ptr;

               //sort these tokens in order
               if(strcmp(left_chunk->data, right_chunk->data) > 0)
               {
                    break;
               }

               temp_kv = *right_key;
               *(right_key) = *(left_key);
               *(left_key) = temp;
               j--;
    }
  }
}
