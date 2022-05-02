#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct semaphores {
    /**
     * mutex    - semaphore for critical when atom's processes starts
     * oxygen   - signals that oxygen atom is ready to initiate reaction
     * hydrogen - posted by oxygen atom and allow 2 hydorgen atoms to react
     * logger   - allow only 1 process to use _log[helpers.c:82 void _log] function
     * ready    - is posted by hydrogen processes to signal oxygen that they are ready to react
     * molecule_creation - is used to synchronize hydrogen with oxygen process
     * molecule_creating - is used to synchronize oxygen with hydrogen processes
     * molecule_created - signalizes that all expected molecules are created and release unused atoms
     */
    sem_t* mutex;
    sem_t* oxygen;
    sem_t* hydrogen;
    sem_t* popper;
    sem_t* logger;
    sem_t* ready;
    sem_t* molecule_creation;
    sem_t* molecule_creating;
    sem_t* molecules_created;
} semaphores;

semaphores* semaphores_init()
{
    /**
     * Initialize semaphores, unlinking semaphores.
     */
    semaphores* semaphore = malloc(sizeof(semaphores));

    if ((semaphore->mutex = sem_open("mutex", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->oxygen = sem_open("oxygen", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->hydrogen = sem_open("hydrogen", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->popper = sem_open("popper", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->ready = sem_open("ready", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->molecule_creation = sem_open("molecule_creation", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->molecule_creating = sem_open("molecule_creating", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->molecules_created = sem_open("molecules_created", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((semaphore->logger = sem_open("logger", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_unlink("mutex");
    sem_unlink("oxygen");
    sem_unlink("hydrogen");
    sem_unlink("popper");
    sem_unlink("ready");
    sem_unlink("molecule_creation");
    sem_unlink("molecule_creating");
    sem_unlink("molecules_created");
    sem_unlink("logger");
    return semaphore;
}

void semaphores_destroy(semaphores* semaphores)
{
    /**
     * Destroy semaphores.
     * Free memory used by semaphores.
     */
    sem_close(semaphores->mutex);
    sem_close(semaphores->oxygen);
    sem_close(semaphores->hydrogen);
    sem_close(semaphores->popper);
    sem_close(semaphores->ready);
    sem_close(semaphores->molecule_creation);
    sem_close(semaphores->molecule_creating);
    sem_close(semaphores->molecules_created);
    sem_close(semaphores->logger);
    free(semaphores);
}