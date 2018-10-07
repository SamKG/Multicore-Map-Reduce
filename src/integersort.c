/*
     Daniel Pattathil - CS 416
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

int main(int argc, char** argv){

}

int map(int offset, int size, int* countreturn){
     int i, j;
     void* ptr = general_shm_ptr;
     int chunksz = sizeof(DataChunk);
     int keyvalsz = sizeof(key_value);
     key_value temp_kv;
     key_value keyvalues[size];

     //This is creating an array of KeyValue pairs with offset values to chunks
     for(i = 0; i < size; i++)
     {
          keyvalues[i].key_offset = (offset + (i * chunksz));
          keyvalues[i].value = 1;
     }

     //This is to sort the array of KeyValue pairs
     DataChunk* next_chunk;
     DataChunk* prev_chunk;

     for (i = 1 ; i < size; i++) {
          j = i;
          while (j > 0){
               next_chunk = keyvalues[j].key_offset + ptr;
               prev_chunk = keyvalues[j-1].key_offset + ptr;
               next_int = atoi((char*)next_chunk->data);
               prev_int = atoi((char*)prev_chunk->data);

               //sort these tokens in order
               if(next_int > prev_int){ break; }
               temp_kv = keyvalues[j];
               keyvalues[j] = keyvalues[j-1];
               keyvalues[j] = temp_kv;
               j--;
          }
     }

     //this compresses the array if there are same values within this chunk
     int index;
     int count = 0;
     for(i = 0; i < size; i++){
          index = i;
          count++;
          prev_chunk = keyvalues[index].key_offset + ptr;
          j = i+1;
          while(j < size){
               next_chunk = keyvalues[j].key_offset + ptr;
               if(strcmp((char*)prev_chunk->data, (char*)prev_chunk->data) == 0)
               {
                    keyvalues[index].value+=1;
                    keyvalues[j] == NULL;
               }
               else{
                    i = j;
               }
          }
     }

     //This is the offset for the resulting array of mappped key_value pairs
     int mappedOffset = shm_get_general(count * keyvalsz);
     for(i = 0, j = 0; i < size; i++){
          if(keyvalues[i] == NULL){
               continue;
          }
          else{
               *(ptr + mappedOffset + (j * keyvalsz)) = keyvalues[i];
               j++;
          }
     }

     *countreturn = count;

     return mappedOffset;
}

void reduce(int start, int end, int count, int insert_pos){

     int i, j;
     void* ptr = general_shm_ptr;
     int keyvalsz = sizeof(key_value);
     key_value* temp;
     int total = 0;

     for(i = 0; i < count; i++){
          temp = *(ptr + start + (i * keyvalsz));
          total += temp->value;
     }

     temp->value = total;
     *(ptr+insert_pos) = temp;

     DataChunk* temp_d;
     if(end == 1){
          for(i = 0; i < count; i++){
               temp = *(ptr + start + (i * keyvalsz));
               temp_d = ptr + temp->key_offset;
               for(j = 0; j < temp->value, j++){
                    printf("%s\n", (char*) ptr + temp_d->data);
               }
          }
     }

     return;
}
