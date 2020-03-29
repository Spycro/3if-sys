#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mem.h"

void * heap_base = NULL;// first byte of the heap
void * heap_end  = NULL;// first byte beyond the heap

int mem_initialized = 0;

// initialize the memory manager
void mem_init(void)
{
    assert(mem_initialized == 0);

    // request memory from the kernel
    heap_base = mmap(NULL, 800, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(heap_base != MAP_FAILED);

    heap_end = heap_base + 800 ;

    // create some free blocks: five of 80 bytes and one of 400 bytes
    void *block_ptr = heap_base;
    /*
    for(int i=0; i<5;i++)
    {
        block_ptr = heap_base + 80*i;
        *( (int64_t*)block_ptr ) = 80;
    }
    block_ptr = heap_base + 400;
    *( (int64_t*)block_ptr ) = 400;
    */
    //header
    *( (int64_t*)block_ptr ) = 800;
    //footer
    *( (int64_t*)(block_ptr+792) ) = 800;


    mem_initialized = 1;
}

void * mem_alloc(int64_t length)
{
    assert(mem_initialized == 1);

    // compute actual size of block
    length = (length+7)/8*8 ; // round up to nearest multiple of 8
    length += 16;              // add space for the header and footer


    // heap traversal
    void *  block_ptr ;
    int64_t header ;
    int64_t footer;
    int64_t size;
    char    flags;

    block_ptr = heap_base;
    while(block_ptr < heap_end)
    {
        header = *( (int64_t*)block_ptr );
        flags  = header & 0b111;  // keep only three least significant bits
        size = header & ~0b111;   // discard the three least significant bits

        if( (flags == 0 ) &&      // if current block is free, and
            (size >= length))     // is also large enough, then we have a winner
            break;

        // otherwise we move on to the next block
        block_ptr += size;
    }

    // if the heap  traversal reached this far, then it  means we have
    // found no suitable block, so we should return NULL
    if(block_ptr >= heap_end)
    {
        return NULL;
    }


    /////////////////////////////
    //resizing block
    //check if it is possible (size remaining of at least 16 (header + footer))
    if(size - length > 15){
      int64_t remaining = size - length;
      size = length;
      void* ptr = block_ptr + size;
      *((int64_t*)ptr) = remaining; //writing new header
      *((int64_t*)(ptr + remaining - 8)) = remaining; //writing new footer
    }
    //end resizing
    /////////////////////////////
    flags = 0b001; // mark block as taken
    header = size | flags;
    footer = header;
    *( (int64_t*)block_ptr ) = header ; // write header back into the block
    *( (int64_t*)(block_ptr + size - 8)) = footer; //write footer back into block
    return block_ptr + 8 ; // skip header
}

void mem_release(void *ptr)
{
    assert( mem_initialized == 1);
    assert( ((int64_t)ptr % 8) == 0 ); // sanity check

    // TODO: your deallocation algorithm goes here
    //checking
    if(ptr==NULL){
      printf("bad pointer\n");
      return;
    }

    //getting to the header :
    ptr = ptr - 8;
    //getting header
    int64_t header = *( (int64_t*)ptr );
    int64_t flags  = header & 0b111;  // keep only three least significant bits
    int64_t size = header & ~0b111;   // discard the three least significant bits
    int64_t footer = *( (int64_t*)(ptr+size - 8) );
    //sanity check
    assert(header == footer);
    //checking if th memory if in use
    if(flags == 0b1){
      //memory in use
      flags = 0b0; //marking memory free
    }
    else{
      printf("memory was free\n");
    }

    //new header/footer
    header = size | flags;
    footer = header;
    //writing both back in
    *( (int64_t*)ptr ) = header ;
    *( (int64_t*)(ptr + size - 8)) = footer;

    //memory is released now

    //Checking for possible fuse of memory block into bigger block
    if(!((ptr + size) >= heap_end)){ //checking if the block is not at the end of the heap
      void * next_block_header_ptr = (ptr + size);
      int64_t next_block_header = *((int64_t *)next_block_header_ptr);
      int64_t next_block_size = next_block_header & ~0b111;
      int64_t next_block_flags = next_block_header & 0b111;

      if(next_block_flags == 0b0){
        //need to fuse block
        size = size + next_block_size;
        header = size | flags;
        footer = header;
        *( (int64_t*)ptr ) = header ;
        *( (int64_t*)(ptr + size - 8)) = footer;
      }
    }
    //check if beginning of heap or not
    if(!(ptr <=heap_base)){
      void * previous_block_footer_ptr = ptr - 8;
      int64_t previous_block_footer = *((int64_t *)previous_block_footer_ptr);
      int64_t previous_block_size = previous_block_footer & ~0b111;
      int64_t previous_block_flags = previous_block_footer & 0b111;

      if(previous_block_flags == 0b0){
        //moving ptr to new head of block
        ptr = ptr - previous_block_size;
        size += previous_block_size;

        header = size | flags;
        footer = header;
        *( (int64_t*)ptr ) = header ;
        *( (int64_t*)(ptr + size - 8)) = footer;
      }
    }




    return;
}

void mem_show_heap(void)
{
    assert( mem_initialized == 1);

    void * block_ptr = heap_base;

    printf("heap_base = %p\n",heap_base);
    while(block_ptr < heap_end)
    {
        int64_t header = *( (int64_t*)block_ptr );
        char    flags  = header & 0b111;  //   keep only three least significant bits
        int64_t size   = header & ~0b111; // discard the three least significant bits
        int64_t footer = *( (int64_t*)(block_ptr+size-8));
        assert(header == footer);

        if( (size < 8) ||
            (size%8 != 0) )
        {
            printf("error: block at %p has incorrect size %ld\n",block_ptr,size);
            exit(1);
        }

        printf("  block at %p: header=0x%08lx footer=0x%08lx size=%ld flags=%d (%s)\n",
               block_ptr,header,footer, size,flags,
               (flags ? "taken" : "free")
               );

        block_ptr += size; // move on to next block
    }

    printf("heap_end = %p\n",heap_end);

    //sanity check: a full heap traversal should reach *exactly* the end
    assert( block_ptr == heap_end);
}
