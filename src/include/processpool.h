#include <types.h>
void start_worker(ProcessPool *pool);
ProcessPool *new_process_pool(char *name,int number_workers);
void destroy_process_pool(ProcessPool* pool);
;extern const int PROCESS_POOL_SIZE;
