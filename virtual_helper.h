#ifndef VIRTUAL_HELPER_H
#define VIRTUAL_HELPER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
 
void * virtual_sbrk(int32_t increment);

/*
uint8_t set_bit(uint8_t original, int position, int is_start , int is_free)
modify 8 bit integer depend on wheter block is free or allocated, whether block
is head or body.
->return modifiy 8 bit integer 
*/
uint8_t set_bit(uint8_t original, int position, int is_start , int is_free);

/*
uint8_t is_start(uint8_t number, int position)
-> check whether block is head or not
-> return 1 yes, 0 no
*/
uint8_t is_start(uint8_t number, int position);

/*
uint8_t is_free(uint8_t number, int position)
-> check whether block is free or allocated
-> return 1 allocated, 0 free
*/
uint8_t is_free(uint8_t number, int position);

/*
int get_init_size(void* heapstart)
Calculate init size 
-> return init size
*/
int get_init_size(void* heapstart);

/*
int get_size(void* ptr , int index , int min )
Calculate size of block in bytes
-> return size of block in bytes
*/
int get_size(void* ptr , int index , int min );

/*
int block_size(void* ptr , int index , int min )
Calculate size of block
-> return size of block
*/
int block_size(void* ptr,int index, int min);

/*
int merge_block(int merge_size, int min_block, void* start_block)
merge free block
-> return 0 if successful, return 1 if all block unable to merge
*/
int merge_block(int merge_size, int min_block, void* start_block);

#endif