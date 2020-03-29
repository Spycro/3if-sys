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
  int descripteur;
  char c;
  for(int i=1; i<argc ; i++)
  {
    descripteur = open(argv[i], O_RDONLY);
    if(descripteur == -1){
      printf("Problem with file: %s\n", argv[i]);
      exit(1);
    }
    char nbRead = 1;
    while(nbRead){
      nbRead = read(descripteur, &c, sizeof(char));
      write(1, &c, sizeof(char));
    }
    close(descripteur);
  }

  return 0;
}
