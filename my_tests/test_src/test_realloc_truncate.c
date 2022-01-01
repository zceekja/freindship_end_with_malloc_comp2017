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
    init_allocator(virtual_heap, 5, 1);
    void* data =virtual_malloc(virtual_heap,4);
    void* data3 = virtual_malloc(virtual_heap,4);

    char* data5 = data3;
    * data5 = 'x';
    data5++;
    * data5 = 'x';
    data5++;
    * data5 = 'x';
    data5++;
    * data5 = 'x';
    virtual_free(virtual_heap, data3);
    void* data2 = virtual_malloc(virtual_heap,2);
    void* data6 = virtual_malloc(virtual_heap,2);
    virtual_free(virtual_heap, data2);
    char* data1 = data;
    * data1 = 'h';
    data1++;
    * data1 = 'e';
    data1++;
    * data1 = 'l';
    data1++;
    * data1 = 'l';
    void* data4 = virtual_realloc(virtual_heap,data,2);
    printf("%c",*(char*)data4);
    data4++;
    printf("%c",*(char*)data4);
    data4++;
    printf("%c",*(char*)data4);
    data4++;
    printf("\n");
    return 0;
}
