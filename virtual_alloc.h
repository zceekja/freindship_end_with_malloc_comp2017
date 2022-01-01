#ifndef VIRTUAL_ALLOC_H
#define VIRTUAL_ALLOC_H

#include "virtual_helper.h"

/*
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) 
(1) change size of virtual heap to 0
(2) change size of virtual help to size_of_data + size_of_overhead
(2) set first byte of heap to min_block
(3) set data structure 
    - Each min-block track with 2 bits
    - Total number of block is init-size divide by min-size
    - 11 represent block is head and allocated
    - 10 represent block is head and unallocated
    - 00 represent block is body
    note: I can improve overhead by only use 1 bit for body block, but this require  
    a lot more work.
 */
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

/*
void * virtual_malloc(void * heapstart, uint32_t size) 
(1) find the smallest block in system that equal or larger than request size
(2) If unable to find such block, return NULL
(2) Otherwise, set data structure and return an address of block.
*/
void * virtual_malloc(void * heapstart, uint32_t size);

/*
int virtual_free(void * heapstart, void * ptr) 
(1) check whether ptr is valid or not
(2) If not valid, or not an allocated block return 1
(3) Otherwise, free that block
(4) Recursive merge
*/
int virtual_free(void * heapstart, void * ptr);

/*
void * virtual_realloc(void * heapstart, void * ptr, uint32_t size)
(1) size > init size return NULL
(2) ptr is not valid block or free return NULL
(3) size is 0, free and return NULL
(4) new block <= old block, free old block, malloc new block
and move memory from old block to new block (truncate)
(5) new block > old block, pretend that old block is freed,
and check whether there is availiable block. If not available 
do nothing and return NULL.
*/
void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

/*
void virtual_info(void * heapstart)
(1) print heap information
*/
void virtual_info(void * heapstart);

#endif