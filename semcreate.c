//usage : semcreate [-e] [-i initvalue]

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    int options, op_flags;
    unsigned int init_count = 1;    //Default
    sem_t *new_sem;
    op_flags = O_CREAT;    // POSIX named semaphores don't require explicit RD/WR perm.
    while( (options = getopt(argc, argv, "ei:")) != -1)
    {
        switch(options)
        {
            case 'e':
                op_flags |= O_EXCL;
                break;
            case 'i':
                init_count = atoi(optarg);
                break;
        }
    }
    //mismatch of args ; Check argindex (argc -1 is first non opt arg)
    if(optind != argc -1) {
        printf("ERROR in usage\n");
        exit(0);
    }
    /**
     * sem_open : last 2 args are considered only if O_CREAT is used.
     *            it'll be ignored if the named sem already exists even if O_CREAT is used.
     */
    new_sem = sem_open(argv[optind], op_flags, 0666, init_count);
    sem_close(new_sem); //this will be implicity accomplished by return as well.
    return 0;
}

