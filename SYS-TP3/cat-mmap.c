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
  for(int i=1; i<argc ; i++)
  {
    descripteur = open(argv[i], O_RDONLY);
    if(descripteur == -1){
      printf("Problem with file: %s\n", argv[i]);
      exit(1);
    }
    struct stat buf;
    fstat(descripteur, &buf);

    char * fichier = mmap(NULL, buf.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, descripteur, 0);
    if(fichier == MAP_FAILED){
      printf("Probleme d'allocation de memoire\n");
      exit(1);
    }
    write(1, fichier, buf.st_size);

    close(descripteur);
  }

  return 0;
}
