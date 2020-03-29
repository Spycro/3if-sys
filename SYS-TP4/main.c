#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mem.h"
#include "test.h"

int main()
{
  // initialize the allocator
  mem_init();

  /*char *p = mem_alloc( 42 );
  assert( p != NULL ); // check whether the allocation was successful
  printf("allocated 42 bytes at %p\n", p);
  mem_show_heap();*/

  //char ** p = successiveTest(7, 42);
  /*char ** p = successiveTest(6, 42);
  mem_release(p[5]);
  mem_show_heap();
  p[5] = mem_alloc(42);
  mem_show_heap();*/
  printf("[*]Beginning state\n");

  mem_show_heap();
  printf("\n[+]Begin test 1 - allocate 200 then 300\n");
  test2_2();
  printf("\n[+]Begin test 2 - allocate exactly 800 - 16\n");
  allocateExactMemory();
  printf("\n[+]Begin test 3 - allocate 5 times 64 bytes then release\n");
  successiveTest(5, 64);
  printf("\n[+]Begin test 4 - allocate 0\n");
  allocateNothing();
  printf("\n[+]Begin test 5 - allocate 64 then 64; free the first one then the second one\n");
  allocateSurrounded();
  printf("\n[*]end state\n");

  mem_show_heap();

  //assert(p != NULL);
}
