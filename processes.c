#include <unistd.h>
#include "shmem.c"
#include "semaphores.c"

int fork_processes(int N, int* id) {
    /**
     * Create N processes
     */
    for(int i = 1 ; i<=N; i++){
        *id = i;
        if(fork()==0) return 0;
    }

    return getpid();
}
