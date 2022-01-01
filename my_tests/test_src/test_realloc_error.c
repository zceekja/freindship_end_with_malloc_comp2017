#include "../../virtual_alloc.h"

#define MEMORY_CAPACITY 9000000
char global_mem[MEMORY_CAPACITY] = {0};
void * virtual_heap =  &global_mem;
void * virtual_break = &global_mem;
void * virtual_sbrk(int32_t increment) {
    void* ret_address = virtual_break;
    virtual_break += increment;
    return  ret_address;
}

int main() {
    // Your own testing code here
    init_allocator(virtual_heap, 14, 10);
    void* data1 =virtual_malloc(virtual_heap,5);
    void* data2 = virtual_realloc(virtual_heap,data1,300000);
    if(data2 == NULL){
        printf("Unable to allocate data\n");
    }
    return 0;
}
