#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "bag.h"

typedef struct {
    int value;
} boxed_int_t;



typedef struct {
    int tnum; // thread number in 0 .. N-1
    bag_t *bag;
    boxed_int_t *sum;
    // TODO: add other fields here
} consumer_arg_t;

// each consumer thread runs this function
void *consumer(void *arg)
{
    consumer_arg_t *carg=(consumer_arg_t *)arg;
    // printf("consumer %d: start\n",carg->tnum);

    while(1)
    {
        boxed_int_t *box = bb_take(carg->bag);
        assert( box != NULL );

        carg->sum->value += box->value;
    }

    // printf("consumer %d: end\n",carg->tnum);
    return NULL; // dummy return to comply with required signature
}


typedef struct {
    int tnum; // thread number in 0 .. N-1
    bag_t *bag;
    // TODO: add other fields here
} producer_arg_t;


// each producer thread runs this function
void *producer(void *arg)
{
    producer_arg_t *parg=(producer_arg_t *)arg;

    // printf("producer %d:start \n",parg->tnum);

    int count;
    for( count = 0 ; count < parg->tnum+1 ; count ++)
    {
        boxed_int_t *box = malloc(sizeof(boxed_int_t));
        assert( box != NULL );
        box->value = 1;

        bb_add(parg->bag, box);
    }

    // printf("producer %d:end\n",parg->tnum);

    return NULL; // dummy return to comply with required signature
}

int main(int argc, char ** argv)
{
    assert(argc == 3);

    int N = atoi( argv[1] );
    assert( N > 0);

    pthread_t prod[N];
    pthread_t cons[N];

    int S = atoi( argv[2] );
    assert( S > 0 );

    // shared container
    bag_t *bag=bb_create(S);
    assert(bag != NULL);

    int tnum;
    for( tnum = 0 ; tnum < N ; tnum++)
    {
        producer_arg_t *parg=malloc( sizeof(producer_arg_t) );
        assert( parg != NULL);

        parg->tnum = tnum;
        parg->bag  = bag;

        pthread_create(&prod[tnum], NULL, producer, parg);
    }

    // shared result
    boxed_int_t *sum = malloc( sizeof(boxed_int_t) );
    assert(sum != NULL);

    for( tnum = 0 ; tnum < N ; tnum++)
    {
        consumer_arg_t *carg = malloc( sizeof(consumer_arg_t) );
        assert( carg != NULL );

        carg->tnum  = tnum;
        carg->bag   = bag;
        carg->sum   = sum;

        pthread_create(&cons[tnum],NULL, consumer, carg);
    }
    sleep(2);

    printf("expected sum=%d\n",N*(N+1)/2);

    printf("computed sum=%d\n", sum->value);

    return 0;
}
