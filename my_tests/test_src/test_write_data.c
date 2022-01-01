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
    void* data =virtual_malloc(virtual_heap,1500);
    char* data1 = data;
    * data1 = 'h';
    data1++;
    * data1 = 'e';
    data1++;
    * data1 = 'l';
    data1++;
    * data1 = 'l';
    data1++;
    *data1 = 'o';
    data1++;
    *data1 = '\n';
    data1++;
    * data1 = '\0';
    printf("%s",(char*)data);
    return 0;
}
