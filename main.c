#include "processes.c"
#include <sys/wait.h>
#define min(a,b) (((a)<(b))?(a):(b))

void common_part(int id, arguments* args, semaphores* sems, shared_memory* shm, atom_queue* queue, element type)
{
    /**
     * bool extra flag for the case when the process(es) is/are not gonna be used to create new molecule
     * Steps:
     *      1. Create atom attached to the process
     *      2. Wait for the semaphore mutex that allows access to critical section (count how many atoms are created)
     *      3. Create atom -> wait -> Queue
     *      4. Create atom (see below)
     */
    bool extra = false;
    atom* at = create_atom(id, type);
    sem_wait(sems->mutex);

    if ((type == HYDROGEN && shm->hydrogen_created >= args->expected * 2) || (type == OXYGEN && shm->oxygen_created >= args->expected))
        extra = true;

    type == OXYGEN ? shm->oxygen_created++ : shm->hydrogen_created++;

    if (shm->oxygen_created >= args->NO && shm->hydrogen_created >= args->expected * 2) {
        sem_post(sems->oxygen);
        sem_post(sems->popper);
    }
    sem_post(sems->mutex);

    sem_wait(sems->logger);
    log_(CREATED, type, at->id, 0, &shm->line, sems->logger);
    args->TB != 0 ? usleep(rand() % args->TI*1000) : usleep(0);

    sem_wait(sems->logger);
    insert_atom(at, queue);
    log_(GO_QUEUE, type, at->id, 0, &shm->line, sems->logger);

    if (extra)
    {
        /**
         * wait for all molecules to be created and log that atom is unused
         */
        sem_wait(sems->popper);
        atom *at2 = pop_atom(queue);
        sem_post(sems->popper);
        sem_wait(sems->molecules_created);
        sem_wait(sems->logger);
        log_(UNUSED, type, at2->id, 0, &shm->line, sems->logger);
        free(at);
        sem_post(sems->molecules_created);
        exit(0);
    }

    // molecule creation part

    if (type == OXYGEN)
    {
        sem_wait(sems->popper);
        atom* at2 = pop_atom(&shm->OXYGEN_QUEUE);
        sem_post(sems->popper);
        /**
         * wait for oxygen semaphore to start molecule creation process
         * post 2 semaphores to allow 2 hydrogen processes
         * synchronise with hydrogen processes
         */
        sem_wait(sems->oxygen);
        shm->molecule_id++;

        sem_wait(sems->logger);
        log_(IN_USE, type, at2->id, shm->molecule_id, &shm->line, sems->logger);

        sem_post(sems->hydrogen);
        sem_post(sems->hydrogen);

        args->TB != 0 ? usleep(rand() % args->TB*1000) : usleep(0);

        sem_post(sems->molecule_creation);
        sem_post(sems->molecule_creation);

        sem_wait(sems->molecule_creating);
        sem_wait(sems->molecule_creating);

        sem_wait(sems->ready);
        sem_wait(sems->ready);
        sem_wait(sems->logger);
        log_(USED, type, at2->id, shm->molecule_id, &shm->line, sems->logger);
        sem_post(sems->oxygen);
        if (shm->molecule_id == args->expected)
        {
            sem_post(sems->molecules_created);
        }
    }
    else if (type == HYDROGEN)
    {
        sem_wait(sems->popper);
        atom* at2 = pop_atom(&shm->HYDROGEN_QUEUE);
        sem_post(sems->popper);

        sem_wait(sems->hydrogen);
        sem_wait(sems->logger);
        log_(IN_USE, type, at2->id, shm->molecule_id, &shm->line, sems->logger);

        sem_wait(sems->molecule_creation);

        sem_post(sems->molecule_creating);

        sem_wait(sems->logger);
        log_(USED, type, at2->id, shm->molecule_id, &shm->line, sems->logger);
        sem_post(sems->ready);
        sem_post(sems->logger);
    }
    free(at);
    exit(0);
}

int main(int argc, char **argv)
{
    /**
     * function starts with removing proj2.out file if it exists
     * read arguments and validate them
     */
    remove(filename);
    arguments* args = create_arguments();
    if (argc != 5)
    {
        fprintf( stderr, "Wrong arguments amount\n");
        exit(1);
    }
    setbuf(stdout, NULL);
    args->NO = read_arg(argv[1]);
    args->NH = read_arg(argv[2]);
    args->TI = read_arg(argv[3]);
    args->TB = read_arg(argv[4]);
    if (args->NO <= 0 || args->NH <= 0 || args->TI < 0 || args->TB < 0)
    {
        fprintf( stderr, "Arguments should be positive\n");
        exit(1);
    }
    if (args->TI > 1000 || args->TB > 1000)
    {
        fprintf( stderr, "Arguments should be less than 1000\n");
        exit(1);
    }

    semaphores* semaphores = semaphores_init();
    /**
     * opens mutex and logger semaphores
     */
    sem_post(semaphores->mutex);
    sem_post(semaphores->logger);
    if (args->NO > 0) {
        int NO_molecules_expected = args->NO;
        if (args->NH > 1) {
            int NH_molecules_expected = args->NH / 2; // returns rounded to min (5.5 -> 5)
            args->expected = min(NO_molecules_expected, NH_molecules_expected);
        } else {    // post that we can release atoms because will not create any molecule
            sem_post(semaphores->molecules_created);
        }
    } else {
        sem_post(semaphores->molecules_created);
    }
    shared_memory* memory = create_shared_memory();
    create_queue(OXYGEN, &memory->OXYGEN_QUEUE, 1, args);
    create_queue(HYDROGEN, &memory->HYDROGEN_QUEUE, 2, args);

    /**
     * creates NH + NO processes and send them to common part for all processes
     */
    int id_h = 0, id_o = 0;
    if (fork_processes(args->NH, &id_h) == 0) {
        common_part(id_h, args, semaphores, memory, &memory->HYDROGEN_QUEUE, HYDROGEN);
    }
    if (fork_processes(args->NO, &id_o) == 0) {
        common_part(id_o, args, semaphores, memory, &memory->OXYGEN_QUEUE, OXYGEN);
    }

    /**
     * waits for all processes to finish
     * close semaphores
     * empty shared memory
     * free memory
     * and end the program
     */
    while (wait(NULL) != -1);
    semaphores_destroy(semaphores);
    free_arguments(args);
    delete_queue(&memory->OXYGEN_QUEUE);
    delete_queue(&memory->HYDROGEN_QUEUE);
    delete_shared_memory(memory);
    return 0;
}
