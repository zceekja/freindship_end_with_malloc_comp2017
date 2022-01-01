#include "virtual_alloc.h"

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
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {

    int decrement_value = (int)(virtual_sbrk(0) -heapstart); 
    virtual_sbrk(-decrement_value);   //change heap to 0
    uint32_t allocation_size = 1 << initial_size; 
    int total_meta_block =  (1<<(initial_size-min_size))/4;
    if((1<<(initial_size-min_size))%4 != 0){
        total_meta_block++;
    }
    virtual_sbrk(allocation_size + 1 + total_meta_block ); //change heap to data+overhead
    *(uint8_t*)heapstart = min_size;
    heapstart++;
    heapstart += allocation_size;
    //Set data structure
    *(uint8_t*)heapstart = 1;
    total_meta_block--;
    while(total_meta_block){
        heapstart++;
        *(uint8_t*)heapstart =0;
        total_meta_block--;
    }
}

/*
void * virtual_malloc(void * heapstart, uint32_t size) 
(1) find the smallest block in system that equal or larger than request size
(2) If unable to find such block, return NULL
(2) Otherwise, set data structure and return an address of block.
*/
void * virtual_malloc(void * heapstart, uint32_t size){

    int init_size = get_init_size(heapstart);
    int min_block = *(uint8_t*)(heapstart);
    if(size == 0 ){
        return NULL;
    }
    void* start_block = heapstart+ 1+ (1<<init_size); 
    void* current_block = start_block; 
    int n = 0;
    //This loop is to find block size
    while(((size-1) >> n) != 0){
        n++;
    }
    uint8_t required_block = n;  //size that user request
    if(required_block > init_size){
        return NULL;
    }
    if(required_block< min_block){
        required_block = min_block; 
    }

    //size at current iteration
    int smallest_block = required_block;      //size that as close as request size
    int index =0;                             
    int ret_index;

    //This loop is to find the smallest block.
    while(1){    
        if (current_block == virtual_sbrk(0)){
            if(smallest_block == init_size){ //if no block available return null
                return NULL;
            }
            current_block = start_block; 
            index = 0;
            smallest_block++;
        }
        if(is_start(*(uint8_t*)current_block,index%4)== 1){
            if(is_free(*(uint8_t*)current_block,index%4) ==1){
                 index+= 1<<(block_size(current_block,index,min_block)- min_block);
                 current_block = start_block + index/4;
            }
            else {
                if (block_size(current_block,index,min_block) == smallest_block){
                    ret_index = index;
                    break;
                }
                else{
                    index += 1<<(block_size(current_block,index,min_block)- min_block);
                    current_block = start_block + index/4;
                }
             }
        }
    }
    // Set data structure
    *(uint8_t*)current_block = set_bit( *(uint8_t*)current_block,  index%4,  1 , 1);   
    if(smallest_block == required_block){
        return heapstart+1+(ret_index)*(1<<min_block);//f
     }
    index += 1 <<(required_block - min_block);
    current_block = start_block + index/4;
    *(uint8_t*)current_block = set_bit( *(uint8_t*)current_block,  index%4,  1 , 0);
    index += 1 <<(required_block - min_block);
    current_block = start_block + index/4;
    while(1){
        if(smallest_block-1 == required_block){
            break;
        }
        else{
            *(uint8_t*)current_block = set_bit( *(uint8_t*)current_block,  index%4,  1 , 0);
            required_block++;
            index += 1 <<(required_block - min_block);
            current_block = start_block + index/4;
        }
    }
    return heapstart+1+(ret_index)*(1<<min_block);   
}

/*
int virtual_free(void * heapstart, void * ptr) 
(1) check whether ptr is valid or not
(2) If not valid, or not an allocated block return 1
(3) Otherwise, free that block
(4) Recursive merge
*/
int virtual_free(void * heapstart, void * ptr) {
    
    int init_size = get_init_size(heapstart);
    int min_block = *(uint8_t*)(heapstart);
    void* start_block = heapstart+ (1<<init_size) +1; 
    void* current_block = start_block; 
    int index;
    int to_free_bytes_offset = (int)(ptr-heapstart)-1;
    int to_free_index = to_free_bytes_offset/ (1<<min_block);
    current_block = start_block + to_free_index/4;
    if( to_free_bytes_offset% (1<<min_block) != 0){
        return 1;
    }
    else if(is_start(*(uint8_t*)current_block, to_free_index%4)== 0){
        return 1;
    }
    else if (is_free(*(uint8_t*)current_block, to_free_index%4)== 0){
        return 1;
    }
    *(uint8_t*)current_block = set_bit( *(uint8_t*)current_block,  to_free_index%4,  1 , 0);
    index = to_free_index;
    int merge_size = block_size(current_block,index, min_block);
    //recursive merge
    while(1){ 
        merge_size++;
        if(merge_block(merge_size,min_block,start_block) == 1 ){
            break;
        }
    }
    return 0;
}

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
void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {

    if (ptr == NULL){
        return NULL;
    }
    int init_size = get_init_size(heapstart);
    int min_block = *(uint8_t*)(heapstart);
    void* start_block = heapstart+ (1<<init_size) +1; 
    void* current_block = start_block; 
    int num = 0;
    //This loop is to get required block size
    while(((size-1) >> num) != 0){
        num++;
    }
    uint8_t required_size = num;
    if(required_size< min_block){
        required_size = min_block;
    }
    int to_relocate_offset = (int)(ptr-heapstart)-1;
    int to_relocate_index = to_relocate_offset/ (1<<min_block);
    current_block = start_block + to_relocate_index/4;
    void* relo_block = current_block;
    int relo_index = to_relocate_index%4;
    //if request size more than init size return NULL
    if(required_size> init_size){
        return NULL;
    }
    // if ptr block is not head return NULL
    if(is_start(*(uint8_t*)current_block, to_relocate_index%4)== 0){
        return NULL;
    }
    // if ptr block is not allocated return null
    if (is_free(*(uint8_t*)current_block, to_relocate_index%4)== 0){
        return NULL;
    }
    // if size is 0 free and return null
    if (size == 0){
        virtual_free(heapstart, ptr);
        return NULL;
    }
    // if size <= original, free previous ptr,malloc new size and move data 
    if (required_size <= block_size(current_block,to_relocate_index,min_block)){
        virtual_free(heapstart,ptr);
        void* ret = virtual_malloc(heapstart,size);
        memmove(ret,ptr,size);
        return ret;
    }
    // if size > original, first check wheather it is possible to fit new size in our heap 
    if (required_size > block_size(current_block,to_relocate_index,min_block)){
      
        int smallest_block = required_size;
        int required_bytes = 1 << smallest_block;
        int current_bytes =0;
        current_block = start_block;
        int jump_amount = smallest_block - min_block;
        void* ret;
        int index=0;
   
        while(1){
            if (current_block >= virtual_sbrk(0)){
               //if no block available return null
               if(smallest_block == init_size){
                   return NULL;
               }
               current_block = start_block; 
               current_bytes = 0;
               index = 0;
               smallest_block++;
               jump_amount++;
               required_bytes = 1 << smallest_block;
            }
            // case where original block is inside calculating block, pretend original block is freed.
            if(current_block <= relo_block && relo_block < (void*)(current_block+(1<<jump_amount)/4)){
                void* exit_ptr =current_block+(1<<jump_amount)/4;
                while(1){
                    if (current_bytes == required_bytes){
                       virtual_free(heapstart,ptr);
                        ret = virtual_malloc(heapstart,size);
                        memmove(ret, ptr, size);
                        return ret;
                    }
                    else if(current_block>= exit_ptr){
                        current_bytes =0;
                        break;
                    }
                    else if(current_block == relo_block && index%4 == relo_index){
                        current_bytes += get_size(current_block,index,min_block);
           
                    }else if (is_start(*(uint8_t*)current_block,index%4)== 1 &&
                        is_free(*(uint8_t*)current_block,index%4) ==0){
                        current_bytes += get_size(current_block,index,min_block);
                        
                    }
                    index += (1<<(block_size(current_block,index,min_block)- min_block));
          
                    current_block = start_block + index/4;
                }
            }
             // case where original block is outside calculating block.
            else if(is_start(*(uint8_t*)current_block, index%4)== 1&&
                is_free(*(uint8_t*)current_block,index%4) ==0){
                    if (block_size(current_block,index,min_block) == smallest_block){
                        virtual_free(heapstart,ptr);
                        ret = virtual_malloc(heapstart,size);
                        memmove(ret, ptr, size);
                        return ret;
                    }
                    else{
                        index +=(1 << jump_amount);
                        current_block = start_block +index/4;
                    }
            }
            // if block is not allocated.
            else {

                index +=(1 << jump_amount);
                current_block = start_block +index/4;
            }
        }
    }
    return NULL;

}

/*
void virtual_info(void * heapstart)
(1) print heap information
*/
void virtual_info(void * heapstart) {

    int init_size = get_init_size(heapstart);
    int min_size = *(uint8_t*)(heapstart);
    void* current_block = heapstart+ (1<<init_size) +1;
    int index =0;

    while(1){
        if (is_start(*(uint8_t*)current_block,  index%4) == 1){
            if (is_free(*(uint8_t*)current_block,  index%4) == 1){
                printf("allocated %d\n", (get_size( current_block, index ,  min_size)));
            }  
            else {
                printf("free %d\n", (get_size( current_block, index ,  min_size)));
            }
             index++;
            if(index%4==0){
                current_block++;
            }
        }
        else if(current_block >= virtual_sbrk(0)){
            return;
        }
        else{
            index++;
            if(index%4==0){
                current_block++;
            }
        }
    }
}