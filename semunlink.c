//semunlink <name>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    //invalid usage
    if(argc != 2) {
        printf("Error in usage \n");
        exit(0);
    }
    // after the semaphore ref count is 0 it can be removed from the kernel persitence.
    sem_unlink(argv[1]);
    return 0;
}