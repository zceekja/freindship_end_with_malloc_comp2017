#include "virtual_helper.h"

/*
uint8_t set_bit(uint8_t original, int position, int is_start , int is_free)
modify 8 bit integer depend on wheter block is free or allocated, whether block
is head or body.
->return modifiy 8 bit integer 
*/
uint8_t set_bit(uint8_t original, int position, int is_start , int is_free){
    uint8_t ret = original;
    if(position == 0){
        ret = ((ret & ~1) | (is_start << 0));
        ret = ((ret & ~2) | (is_free << 1));
    }
    else if (position == 1){
        ret = ((ret & ~4) | (is_start << 2));
        ret = ((ret & ~8) | (is_free << 3));
    }
    else if (position == 2){
        ret = ((ret & ~16) | (is_start << 4));
        ret = ((ret & ~32) | (is_free << 5));
    }
    else {
        ret = ((ret & ~64) | (is_start << 6));
        ret = ((ret & ~128) | (is_free << 7));
    }
    return ret;
}

/*
uint8_t is_start(uint8_t number, int position)
-> check whether block is head or not
-> return 1 yes, 0 no
*/
uint8_t is_start(uint8_t number, int position){
    uint8_t ret;
    if(position == 0){
        return ret = (number >> 0) & 1; 
    }
    else if (position == 1){
        return ret = (number >> 2) & 1; 
    }
    else if (position == 2){
        return ret = (number >> 4) & 1; 
    }
    else {
        return ret = (number >> 6) & 1; 
    }
}

/*
uint8_t is_free(uint8_t number, int position)
-> check whether block is free or allocated
-> return 1 allocated, 0 free
*/
uint8_t is_free(uint8_t number, int position){
    uint8_t ret;
    if(position == 0){
        return ret = (number >> 1) & 1; 
    }
    else if (position == 1){
        return ret = (number >> 3) & 1; 
    }
    else if (position == 2){
        return ret = (number >> 5) & 1; 
    }
    else {
        return ret = (number >> 7) & 1; 
    }
}

/*
int get_init_size(void* heapstart)
Calculate init size 
-> return init size
*/
int get_init_size(void* heapstart){
    int bytes = virtual_sbrk(0)- heapstart;
    int ret = 0;
    while((bytes-1)>> ret != 1){
        ret++;
    }
    return ret;
}

/*
int get_size(void* ptr , int index , int min )
Calculate size of block in bytes
-> return size of block in bytes
*/
int get_size(void* ptr , int index , int min ){

    int ret =0 ;
    ret += (1<<min);
    index++;
    if(index%4==0){
        ptr++;
    }
    while(1){
        if (is_start(*(uint8_t*)ptr,  index%4) == 1 || (ptr == virtual_sbrk(0))){
            return ret;
        }
        else{
            ret += (1<<min);
            index++;
            if(index%4==0){
                ptr++;
            }
        }
    }
}

/*
int block_size(void* ptr , int index , int min )
Calculate size of block
-> return size of block
*/
int block_size(void* ptr,int index, int min){

    int size = get_size( ptr,index, min);

    int n = 0;
    while(((size-1) >> n) != 0){
        n++;
    }

    return n;
}

/*
int merge_block(int merge_size, int min_block, void* start_block)
merge free block
-> return 0 if successful, return 1 if all block unable to merge
*/
int merge_block(int merge_size, int min_block, void* start_block){

    void* current_block = start_block;
    
    int jump_size = 1<<(merge_size-min_block);
          
    int index =0;
      
    int buddy_index = index+jump_size/2;

    void* buddy_block = start_block+buddy_index/4;

    while(1){              
        if(current_block >= virtual_sbrk(0) || buddy_block >= virtual_sbrk(0)){
          
            return 1;
        }
        if((is_start(*(uint8_t*)current_block,index%4)==1 )&&
        (is_free(*(uint8_t*)current_block,index%4)==0 )&&
        (block_size(current_block,index,min_block)== merge_size-1 )&&
        (is_start(*(uint8_t*)buddy_block,buddy_index%4)==1) &&
        (is_free(*(uint8_t*)buddy_block,buddy_index%4)==0 )&&
        (block_size(buddy_block,buddy_index,min_block)== merge_size-1)){
            *(uint8_t*)buddy_block = set_bit(*(uint8_t*)buddy_block,buddy_index%4,0,0);
            return 0;
        }
        else{
            index += jump_size;
            current_block = start_block + index/4;
            buddy_index = index+jump_size/2;;
            buddy_block = start_block + buddy_index/4;
        }
    }
}