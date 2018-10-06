#include <types.h>
void* start_thread_worker(void* pool_ptr);
ThreadPool* new_thread_pool(char *name,int number_workers);
void destroy_thread_pool(ThreadPool* pool);
extern const int THREAD_POOL_SIZE;
