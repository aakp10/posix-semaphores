//P:C::N:1 ; C follows P
//pc_nx1 <buf_count> <prod_count>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_PROD 100
#define MAX_BUF_SIZE 100

struct shared{
    pthread_mutex_t prod_mutex;
    int buf_count;
    int buffer[MAX_BUF_SIZE];
    int next_item;
    int next_index;
};

struct shared sync_data;

void*
producer(void *arg)
{
    int *count = (int*)arg;
    while(1)
    {
        //assert lock on next_item and next_index processing
        pthread_mutex_lock(&(sync_data.prod_mutex));
            if(sync_data.next_index < sync_data.buf_count) {
                sync_data.buffer[sync_data.next_index] = sync_data.next_item;
                printf("Writing %d at %d \n", sync_data.buffer[sync_data.next_index], sync_data.next_index);
                sync_data.next_index++;
                sync_data.next_item++;
            }
            else {
                /* !UNLOCK THIS OTHERWISE OTHER THREADS CAN'T BREAK OUT OF WHILE!*/
                pthread_mutex_unlock(&(sync_data.prod_mutex));
                return NULL;
            }
        pthread_mutex_unlock(&(sync_data.prod_mutex));
        *count += 1; //no two threads can access same count index, so move this out of CS
    }
    return NULL;
}

void*
consumer(void *arg)
{
    for(int i = 0; i < sync_data.buf_count; i++)
    {
        printf("buffer[%d] = %d\n", i, sync_data.buffer[i]);
    }
    return NULL;
}

void
init_sync_data(int buf_count)
{
    //sync_data.prod_mutex = PTHREAD_MUTEX_INITIALIZER; // same as 
    pthread_mutex_init(&(sync_data.prod_mutex), NULL);
    sync_data.buf_count = buf_count;
    sync_data.next_index = 0;
    sync_data.next_item = 0;
}

int main(int argc, char *argv[])
{
    //producer count
    int prod_count;
    int buf_count;
    pthread_t prod_tid[MAX_PROD], cons_tid;
    int count_produced[MAX_PROD];
    //usage error
    if(argc != 3) {
        printf("Error in usage");
        exit(0);
    }

    buf_count = atoi(argv[1]);
    prod_count = atoi(argv[2]);

    //initialize sync data
    init_sync_data(buf_count);
    //create prod_count producer threads
    for(int i = 0; i < prod_count; i++)
    {
        count_produced[i] = 0;
        pthread_create(prod_tid + i, NULL, producer, (void*)(count_produced + i)/*count of elements added by this thread*/);
    }
    for(int i = 0; i < prod_count; i++)
    {
        pthread_join(prod_tid[i], NULL);
        //print the count of ele inserted by the joining thread
        printf("thread%d wrote %d ele\n", i, count_produced[i]);
    }
    //create 1 consumer thread after producers have been joined
    pthread_create(&cons_tid, NULL, consumer, NULL);
    pthread_join(cons_tid, NULL);
    return 0;
}