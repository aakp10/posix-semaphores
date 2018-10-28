/**
 * open already existing semaphore, get its value, print
 * semgetvalue <name of already existing semaphore>
 */
#include <stdio.h>
#include <stdlib.h>
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
    // O_CREAT is not required as an already existing semaphore is being opened by this process
    // using it would require two more args to be specified which will not be used since the semaphore already exists
    // using O_EXCL hinders the sole purpose.
    new_sem = sem_open(argv[1], 0);
    sem_getvalue(new_sem, &value);
    printf("%s->value = %d\n", argv[1], value);
    return 0;
}