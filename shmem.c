#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include "helpers.c"

/* MAP for check processes status
 * look 63 lines in main.c
 */
#define OXYGEN_READY 0
#define HYDROGEN_READY 1

void create_queue(element el, atom_queue* queue, int id, arguments *args) {
    srand(id);

    if (el == 'O') {
        queue->shmid = shmget(rand() % getpid(), sizeof(atom)* args->NO, IPC_CREAT | 0666);
    }
    if (el == 'H') {
        queue->shmid = shmget(rand() % getpid(), sizeof(atom) * args->NH, IPC_CREAT | 0666);
    }
    if (queue->shmid == -1) {
        perror("shmget");
        exit(1);
    }
    queue->atoms = (atom *)shmat(queue->shmid, NULL, 0);
    if (queue->atoms == (atom *) -1) {
        perror("shmat");
        exit(1);
    }
    queue->size = 0;
}


shared_memory* create_shared_memory()
{
    size_t size = sizeof(shared_memory);
    printf("size: %zu\n", size);
    key_t key = ftok("shmem.c", 'c');
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }
    shared_memory* shm = shmat(shmid, NULL, 0);
    if (shm == (void*)-1)
    {
        perror("shmat");
        exit(1);
    }
    *shm = (shared_memory){0};

    shm->size = 0;
    shm->shmid = shmid;
    shm->ready[OXYGEN_READY] = false;
    shm->ready[HYDROGEN_READY] = false;
    shm->molecule_status[0] = 0;
    shm->molecule_status[1] = 0;
    shm->molecule_status[2] = 0;
    return shm;
}

void delete_shared_memory(shared_memory *shared_memory)
{
    int id = shared_memory->shmid;
    shmdt(shared_memory);
    if (shmctl(id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(1);
    }
}

void delete_queue(atom_queue* queue)
{
    shmctl(queue->shmid, IPC_RMID, NULL);
    shmdt(queue->atoms);
}