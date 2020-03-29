#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

void afficher(char* crible, int n)
{
    // TODO: afficher les nombres premiers plus petits que n
    printf("nombre premier jusqu'a %d: ", n);
    for (int i = 0; i < n; i++) {
      if (crible[i]==1) {
        printf("%d ", crible[i]*i);
      }
    }
    printf("\n");
}

void rayer_multiples(char* crible, int n, int k)
{
    // TODO: rayer (i.e. passer à zéro) tous les multiples de k qui sont plus petits que n
    for (int i = 2; k*i < n; i++) {
      crible[k*i]=0;
    }
}


int main(int argc, char **argv)
{
    int n=1000;
    if(argc>1)
    {
      n = atoi(argv[1]);
      assert( n > 0 );
    }

    //char buffer[1000];


    //Crezation et allocation de crible de taille n
    char * crible = mmap(NULL, sizeof(char)*n, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0,0);

    if(crible == MAP_FAILED){
      printf("Proble d'allocation de memoire\n");
      exit(1);
    }

    for(int i=0; i<n; i++)
    {
        crible[i] = 1;//par défaut: pas encore barré
    }
    //Appel de n-2 enfants
    pid_t pid = 1;
    for(int k=2; k<n; k++)
    {
      pid = fork();
      if(pid == 0){
        rayer_multiples(crible, n, k);
        exit(0);
      }
    }
    pid_t wpid;
    int status;
    printf("[*] Waiting for all child processes to end\n");
    printf("\n");
    fflush(stdout);
    while((wpid = wait(&status)) > 0);
    afficher(crible, n);

    return 0;
}
