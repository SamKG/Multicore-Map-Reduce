/* This file was automatically generated.  Do not edit! */
#include <types.h>
void queue_enqueue(Queue *queue,Node data);
Node queue_dequeue(Queue *queue);
int queue_is_full(Queue *queue);
int queue_is_empty(Queue *queue);
Queue *new_queue(char *name);
void destroy_queue(Queue* queue);
extern const char *QUEUE_NAME_MODIFIER;
extern const int QUEUE_SIZE_T;
