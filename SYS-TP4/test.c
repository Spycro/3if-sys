#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "test.h"
#include "mem.h"

void successiveTest(int64_t times, int64_t size){
  char **p = malloc(sizeof(char *)*times);
  for(int i=0; i<times;i++){
    p[i] = mem_alloc( size );
    assert( p[i] != NULL ); // check whether the allocation was successful
    printf("allocated %ld bytes at %p\n", size,p);
    mem_show_heap();
  }
  for(int i=0; i<times;i++){
    mem_release(p[i]);
  }
  free(p);
  return ;
}

void allocateExactMemory(){
  char * p = mem_alloc(800-16);
  mem_show_heap();
  mem_release(p);
  mem_show_heap();
}

void allocateNothing(){
  char * p = mem_alloc(0);
  mem_show_heap();
  mem_release(p);
  mem_show_heap();
}

void test2_2(){
  char *p = mem_alloc(200);
  mem_show_heap();
  mem_release(p);
  mem_show_heap();
  p = mem_alloc(300);
  mem_show_heap();
  mem_release(p);
  mem_show_heap();
  return;
}


void allocateSurrounded(){
  void *p1 = mem_alloc(64);
  void *p2 = mem_alloc(64);
  mem_show_heap();
  mem_release(p1);
  mem_show_heap();
  mem_release(p2);
}
