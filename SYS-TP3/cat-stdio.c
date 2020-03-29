#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

  char c;
  for(int i=1; i<argc ; i++)
  {
      FILE * current = fopen(argv[i], "r");
      if(current == NULL){
        printf("Problem with file: %s\n", argv[i]);
        exit(1);
      }
      while(!feof(current)){
        c = fgetc(current);
        putchar(c!=EOF?c:'\0');
      }
      fclose(current);
  }

  return 0;
}
