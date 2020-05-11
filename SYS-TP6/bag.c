#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "bag.h"




bag_t * bb_create(int size)
{
    assert(size > 0);

    bag_t *bag=malloc(sizeof(bag_t));
    assert(bag != NULL);

    bag->elem = malloc( size * sizeof(void*));
    assert(bag->elem != NULL);

    bag->size  = size;
    bag->count = 0;
    bag->is_closed = 0;
    bag->sem_add = malloc(sizeof(sem_t));
    bag->sem_rem = malloc(sizeof(sem_t));
    sem_init(bag->sem_add, 0, size); // last item to change
    sem_init(bag->sem_rem, 0, 0); // last item to change

    return bag;
}

void bb_add(bag_t * bag, void * element)
{
    assert(bag != NULL);                 // sanity check
    //Condition bloquante : Le nombre dans dans le sac est egal a la taille
    assert( bag-> is_closed == 0 );   // adding to a closed bag is an error
    sem_wait(bag->sem_add);
    assert( bag->count < bag->size ); // sanity check
    bag->elem[bag->count] = element;
    bag->count += 1;
    sem_post(bag->sem_rem);
}

void * bb_take(bag_t *bag)
{
    assert(bag != NULL); // sanity check
    //Condition bloquante : on retire pas d'un sac vide
    sem_wait(bag->sem_rem);
    if(bag->is_closed && bag->count<=0){
      return NULL;
    }

    assert( bag->count > 0); // sanity check
    bag->count -= 1;
    void *r = bag->elem[bag->count];

    usleep(10);// artificial delay to increase the occurence of race conditions
    sem_post(bag->sem_add);

    return r;
}

void bb_close(bag_t *bag, int N)
{
    assert( bag-> is_closed == 0 );
    bag->is_closed = 1;
    for (size_t i = 0; i < N; i++) {
      sem_post(bag->sem_rem);
    }
}
