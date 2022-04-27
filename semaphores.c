#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct semaphores {
    char *oxygen_name;
    sem_t* oxygen;

    char *hydrogen_name;
    sem_t* hydrogen;

    sem_t* mutex;
    sem_t* ready;
    sem_t* molecule_creation;
    sem_t* molecules_created;
} semaphores;

semaphores* semaphores_init(char* name1, char* name2)
{
    semaphores* semaphore = malloc(sizeof(semaphores));
    if ((semaphore->oxygen = sem_open(name1, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }
    semaphore->oxygen_name = name1;

    if ((semaphore->hydrogen = sem_open(name2, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }
    semaphore->hydrogen_name = name2;

    if ((semaphore->mutex = sem_open("mutex", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
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

    if ((semaphore->molecules_created = sem_open("molecules_created", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }
    sem_unlink(semaphore->oxygen_name);
    sem_unlink(semaphore->hydrogen_name);
    sem_unlink("mutex");
    sem_unlink("ready");
    sem_unlink("molecule_creation");
    sem_unlink("molecules_created");
    return semaphore;
}

void semaphores_destroy(semaphores* semaphores)
{
    sem_close(semaphores->oxygen);
    sem_close(semaphores->hydrogen);
    sem_close(semaphores->mutex);
    sem_close(semaphores->ready);
    sem_close(semaphores->molecule_creation);
    free(semaphores);
}