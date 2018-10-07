/*:\
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
	keyvalues[i].key_offset = ((DataChunk) offset+ (i*chunksz))->data;
        //keyvalues[i].key_offset = (offset + (i * chunksz));
        keyvalues[i].value = 1;
     }

     //This is to sort the array of KeyValue pairs
     //DataChunk* next_chunk;
     //DataChunk* prev_chunk;

     char *next_chunk;
     char *prev_chunk;

     for (i = 1 ; i < size; i++) {
          j = i;
          while (j > 0){
               next_chunk = (char *)keyvalues[j].key_offset + ptr;
               prev_chunk = (char *)keyvalues[j-1].key_offset + ptr;

               //sort these tokens in order
               if(strcmp(next_chunk, prev_chunk) > 0)
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
     int index;
     int count = 0;
     for(i = 0; i < size; i++){
          index = i;
          count++;
          prev_chunk = (char *)keyvalues[index].key_offset + ptr;
          j = i+1;
          while(j < size){
               next_chunk = (char *)keyvalues[j].key_offset + ptr;
               if(strcmp(prev_chunk, next_chunk) == 0)
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
               // *(ptr + mappedOffset + (j * keyvalsz)) = keyvalues[i];
	       // do i have to actually copy? 
	       // use a lock?
               ((key_value)(ptr + mappedOffset + (j*keyvalsz)))->key_offset = keyvalues[i]->key_offset;
	       ((key_value) (ptr + mappedOffset + (j*keyvalsz)))->value = keyvalues[i]->value;
		j++;
          }
     }
     int nodeOffset = shm_get_general(Node);
     (Node) *(ptr + nodeOffset)->operation = Map;
     (Node) *(ptr + nodeOffset)->num_chunks = count;
     (Node) *(ptr + nodeOffset)->data_offset = mappedOffset;
	
     *countreturn = count;

     return nodeOffset;

}

void reduce(int start, int end, int count, int insert_pos){

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
               printf("%s\t%d\n", (char*) ptr + temp_d->data, temp->value)
          }
     }

     return;
}
