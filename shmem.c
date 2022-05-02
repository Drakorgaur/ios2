#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include "helpers.c"

#define OXYGEN_READY 0
#define HYDROGEN_READY 1

void create_queue(element el, atom_queue* queue, int id, arguments *args) {
    /**
     * Creates a queue for the given element in shared memory.
     */
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
    /**
     * Creates shared memory for Oxygen and Hydrogen processes.
     */
    size_t size = sizeof(shared_memory);
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

    shm->line = 0;
    shm->shmid = shmid;
    shm->molecule_id = 0;
    shm->oxygen_created = 0;
    shm->hydrogen_created = 0;
    shm->OXYGEN_QUEUE.size = 0;
    shm->OXYGEN_QUEUE.fake_size = 0;
    shm->HYDROGEN_QUEUE.size = 0;
    shm->HYDROGEN_QUEUE.fake_size = 0;
    shm->ready[OXYGEN_READY] = false;
    shm->ready[HYDROGEN_READY] = false;
    shm->molecule_status[0] = 0;
    shm->molecule_status[1] = 0;
    shm->molecule_status[2] = 0;
    return shm;
}

void delete_shared_memory(shared_memory *shared_memory)
{
    /**
     * Free shared memory by saved shmid.
     */
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
    /**
     * Free queue by saved shmid.
     */
    shmctl(queue->shmid, IPC_RMID, NULL);
    shmdt(queue->atoms);
}