/**
* consumer can't read when buf is empty
* producer can't place when buf is full
* maintain the shared state vars in a struct and use it only after asserting a lock
*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#define MAX_PROD 100
#define MAX_BUF_SIZE 100

/**
* shared struct
* buf
* put_index - to put
* get_index - to get
* mutex/ bin semaphore - CS - accessing buffer and changing state vars
* counting sem - empty buf entries
* counting sem - filled buf entries
*/

struct shared{
    int buffer[MAX_BUF_SIZE];
    int put_index;
    int get_index;
    int buf_count;
    int item;
    sem_t *mutex;
    sem_t *n_empty;
    sem_t *n_filled;
};

struct shared sync_data;

void
init_sync_data(int buf_count)
{
    sync_data.put_index = 0;
    sync_data.get_index = 0;
    sync_data.buf_count = buf_count;
    sync_data.item = 0;
    sync_data.mutex = sem_open("mutex", O_CREAT, 0666, 1);
    sync_data.n_empty = sem_open("nempty", O_CREAT, 0666, buf_count);
    sync_data.n_filled = sem_open("nfilled", O_CREAT, 0666, 0);
}

void*
producer(void *arg)
{
    while(1)
    {
        //check if new item can be produced
        sem_wait(sync_data.n_empty);
        //empty place exists assert lock on the shared buffer
        sem_wait(sync_data.mutex);
        sync_data.buffer[sync_data.put_index] = sync_data.item;
        //increase the next put index
        sync_data.put_index = (sync_data.put_index + 1) % sync_data.buf_count;
        sync_data.item += 1;
        //exit CS
        sem_post(sync_data.mutex);
        //one item ready to read
        sem_post(sync_data.n_filled);
    }
    return NULL;
}

void*
consumer(void *arg)
{
    while(1)
    {
        //check if item is ready to be consumed
        sem_wait(sync_data.n_filled);
        sem_wait(sync_data.mutex);
        int get_index = sync_data.get_index;
        printf("consumer> buf[%d] = %d\n", get_index, sync_data.buffer[get_index]);
        sync_data.get_index = (sync_data.get_index + 1) % sync_data.buf_count;
        sem_post(sync_data.mutex);
        sem_post(sync_data.n_empty);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int prod_count, cons_count;
    int buf_count;
    pthread_t prod_tid[MAX_PROD], cons_tid[MAX_PROD];
    int count_produced[MAX_PROD];
    //usage error
    if(argc != 3) {
        printf("Error in usage");
        exit(0);
    }

    buf_count = atoi(argv[1]);
    prod_count = atoi(argv[2]);
    cons_count = prod_count;
    //initialize sync data
    init_sync_data(buf_count);
    //create prod_count producer threads
    for(int i = 0; i < prod_count; i++)
    {
        count_produced[i] = 0;
        pthread_create(prod_tid + i, NULL, producer, (void*)(count_produced + i)/*count of elements added by this thread*/);
        pthread_create(cons_tid + i, NULL, consumer, NULL);
    }
    for(int i = 0; i < prod_count; i++)
    {
        pthread_join(prod_tid[i], NULL);
        pthread_join(cons_tid[i], NULL);
        //print the count of ele inserted by the joining thread
        printf("thread%d wrote %d ele\n", i, count_produced[i]);
    }
    return 0;
}



