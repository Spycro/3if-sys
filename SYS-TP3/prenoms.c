#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int sexe; // 1=Garcon, 2=Fille
    char prenom[25];
    int annee; // 1900..2018
    int nombre; // d'enfants nés cette année avec ce prénom
} tuple;

int main(int argc, char **argv)
{
    printf("Hello, prenoms\n");
    // Ouverture du fichier
    int fd = open("prenoms.dat", O_RDONLY);
    struct stat buf;
    fstat(fd, &buf);
    printf("nb de tuple : %ld\n", buf.st_size/sizeof(tuple));
    long nbTuple = buf.st_size/sizeof(tuple);
    tuple * data = mmap(NULL, buf.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
      printf("Probleme d'allocation de memoire\n");
      exit(1);
    }
    for (size_t i = 0; i < nbTuple ; i++) {
      if((data+i)->sexe && strcmp((data+i)->prenom,"LUCAS") == 0)
        printf("%d : %s : %d : %d\n", (data+i)->sexe,(data+i)->prenom,(data+i)->annee, (data+i)->nombre);
    }
    return 0;
}
