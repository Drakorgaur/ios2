#include "processes.c"
#define SEMAPHORE1_NAME "/semaphoreO7"
#define SEMAPHORE2_NAME "/semaphoreH7"
#define min(a,b) (((a)<(b))?(a):(b))

int main(int argc, char **argv)
{
    remove(filename);
    arguments* args = create_arguments();
    if (argc < 4)
    {
        printf("Usage: ./proj2 <number of OXYGEN> <number of HYDROGEN> <time to create atom> <time to create molecule>\n");

        return 0;
    }
    setbuf(stdout, NULL);
    args->NO = read_arg(argv[1]);
    args->NH = read_arg(argv[2]);
    args->TI = read_arg(argv[3]);
    args->TB = read_arg(argv[4]);
    if (args->NO < 0 || args->NH < 0 || args->TI < 0 || args->TB < 0)
    {
        printf("Usage: ./proj2 <number of OXYGEN> <number of HYDROGEN> <time to create atom> <time to create molecule>\n"
               "All arguments must be positive integers\n");
        return 0;
    }

    semaphores* semaphores = semaphores_init(SEMAPHORE1_NAME, SEMAPHORE2_NAME);
    sem_post(semaphores->logger);
    if (args->NO > 0) {
        int NO_molecules_expected = args->NO;
        if (args->NH > 1) {
            int NH_molecules_expected = args->NH / 2; // returns rounded to min (5.5 -> 5)
            args->expected = min(NO_molecules_expected, NH_molecules_expected);
        } else {
            sem_post(semaphores->molecules_created);
        }
    } else {
        sem_post(semaphores->molecules_created);
    }
    shared_memory* memory = create_shared_memory();
    create_queue(OXYGEN, &memory->OXYGEN_QUEUE, 1, args);
    create_queue(HYDROGEN, & memory->HYDROGEN_QUEUE, 2, args);

    if (fork() == 0) {
        if (fork() == 0) {

            common_part(args, semaphores->molecules_created, semaphores->ready,
                        &memory->HYDROGEN_QUEUE, HYDROGEN, memory->ready, &memory->line, semaphores->logger);

            exit(0);

        } else {
            common_part(args, semaphores->molecules_created, semaphores->ready,
                        &memory->OXYGEN_QUEUE, OXYGEN, memory->ready, &memory->line, semaphores->logger);

        }
        while (wait(NULL) != -1);


        exit(0);
    } else {
        sem_wait(semaphores->ready);
        sem_post(semaphores->molecule_creation);

        for (int id = 1; id <= args->expected; id++) {
            int pid;
            if (fork_processes(2, &pid) == 0) {
                atom* H1 = pop_atom(&memory->HYDROGEN_QUEUE);
                while (H1->ready);
                sem_wait(semaphores->logger);
                log_(IN_USE, HYDROGEN, H1->id, id, &memory->line, semaphores->logger);

                memory->molecule_status[pid] = 1;
                while (!memory->molecule_status[0]) ;
                while (!memory->molecule_status[(pid % 2) + 1]);
                usleep(rand() % args->TB);
                sem_wait(semaphores->logger);
                log_(USED, HYDROGEN, H1->id, id, &memory->line, semaphores->logger);

                free(H1);
                exit(0);
            } else {
                atom* O = pop_atom(&memory->OXYGEN_QUEUE);
                while (O->ready);
                sem_wait(semaphores->logger);
                log_(IN_USE, OXYGEN, O->id, id, &memory->line, semaphores->logger);

                memory->molecule_status[0] = 1;
                while (!memory->molecule_status[1]) ;
                while (!memory->molecule_status[2]) ;
                usleep(rand() % args->TB);
                sem_wait(semaphores->logger);
                log_(USED, OXYGEN, O->id, id, &memory->line, semaphores->logger);

                free(O);
                wait(NULL);

            }
            memory->molecule_status[0] = 0;
            memory->molecule_status[1] = 0;
            memory->molecule_status[2] = 0;
        }
    }
    sem_post(semaphores->molecules_created);

    while (wait(NULL) != -1);
    semaphores_destroy(semaphores);
    free_arguments(args);
    delete_queue(&memory->OXYGEN_QUEUE);
    delete_queue(&memory->HYDROGEN_QUEUE);
    delete_shared_memory(memory);
    return 0;
}

void common_part(arguments* args, sem_t* molecules_created, sem_t* sem_ready, atom_queue* queue, element type,
                 bool* ready, int* line, sem_t* sem_line)
{
    int id = 0;
    if (type == OXYGEN) {
        if (fork_processes(args->NO, &id) != 0) {
            while (wait(NULL) > 0);
            exit(0);
        }
    } else if (type == HYDROGEN) {
        if (fork_processes(args->NH, &id) != 0) {
            while (wait(NULL) > 0);
            exit(0);
        }
    }
    atom* atm = create_atom(id, type);

    insert_atom(atm, queue);
    int actual = type == OXYGEN ? args->expected : args->expected * 2;
    if ( queue->fake_size > actual ) {
        sem_wait(molecules_created);
        sem_wait(sem_line);
        log_(UNUSED, type, atm->id, 0, line, sem_line);
        free(atm);
        sem_post(molecules_created);
        sem_post(sem_ready);
        exit(0);
    }
    sem_wait(sem_line);
    log_(CREATED, type, id, 0, line, sem_line);
    if (type == OXYGEN) {
        if (queue->fake_size >= args->expected) {
            ready[OXYGEN_READY] = true;
        }
    } else if (type == HYDROGEN) {
        if (queue->fake_size >= args->expected*2) {
            ready[HYDROGEN_READY] = true;
        }
    }
    if (ready[OXYGEN_READY] && ready[HYDROGEN_READY]) {
        sem_post(sem_ready);
    }
    usleep(rand() % args->TI);
    sem_wait(sem_line);
    log_(GO_QUEUE, type, id, 0, line, sem_line);
    atm->ready = true;
}
