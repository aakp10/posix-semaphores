/**
 * open already existing semaphore, sem_post, print cur sem->value
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    sem_t *new_sem;
    int value;
    //check args
    if(argc != 2) {
        printf("Error in usage \n");
        exit(0);
    }

    new_sem = sem_open(argv[1], 0);
    sem_post(new_sem);
    sem_getvalue(new_sem, &value);
    printf(" %d %s->value = %d", getpid(), argv[1], value);
    return 0;
}