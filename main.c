#include "processes.c"
#define SEMAPHORE1_NAME "/semaphoreO7"
#define SEMAPHORE2_NAME "/semaphoreH7"
#define min(a,b) (((a)<(b))?(a):(b))


int main(int argc, char **argv)
{
    arguments* args = create_arguments(argv);
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

    semaphores* semaphores = semaphores_init(SEMAPHORE1_NAME, SEMAPHORE2_NAME);
    shared_memory* memory = create_shared_memory();
    create_queue(OXYGEN, &memory->OXYGEN_QUEUE, 1, args);
    create_queue(HYDROGEN, & memory->HYDROGEN_QUEUE, 2, args);

    if (fork() == 0) {
        if (fork() == 0) {

            common_part(args, semaphores->hydrogen, semaphores->ready,
                        &memory->HYDROGEN_QUEUE, HYDROGEN, memory->ready);

            exit(0);

        } else {
            common_part(args, semaphores->oxygen, semaphores->ready,
                        &memory->OXYGEN_QUEUE, OXYGEN, memory->ready);

        }
        while (wait(NULL) != -1);

        exit(0);
    } else {
        sem_wait(semaphores->ready);
        printf("all processes are ready\n");

        int expected = 0;
        if (args->NO > 0) {
            int NO_molecules_expected = args->NO;
            if (args->NH > 0) {
                int NH_molecules_expected = args->NH / 2; // returns rounded to min (5.5 -> 5)
                expected = min(NO_molecules_expected, NH_molecules_expected);
            }
        } else {
            for (int i = 0; i < args->NO; i++) {
                printf("O %d: not enough H\n", memory->OXYGEN_QUEUE.atoms[i].id);
            }
        }

        sem_post(semaphores->molecule_creation);

        for (int id = 1; id <= expected; id++) {
            int pid;
            if (fork_processes(2, &pid) == 0) {
                atom* H1 = pop_atom(&memory->HYDROGEN_QUEUE);

                while (H1->ready);
                log_(IN_USE, HYDROGEN, H1->id, id);
                memory->molecule_status[pid] = 1;
                while (!memory->molecule_status[0]) ;
                while (!memory->molecule_status[(pid % 2) + 1]) ;
                log_(USED, HYDROGEN, H1->id, id);
                free(H1);
                exit(0);
            } else {
                atom* O = pop_atom(&memory->OXYGEN_QUEUE);
//                printf("O id %d\n", O->id);
                while (O->ready);
                log_(IN_USE, OXYGEN, O->id, id);
                memory->molecule_status[0] = 1;
                while (!memory->molecule_status[1]) ;
                while (!memory->molecule_status[2]) ;
                log_(USED, OXYGEN, O->id, id);
                free(O);
                while (wait(NULL) != -1);
            }
            usleep(rand() % args->TB);
            memory->molecule_status[0] = 0;
            memory->molecule_status[1] = 0;
            memory->molecule_status[2] = 0;
        }
        while (wait(NULL) != -1);
    }
    while (wait(NULL) != -1);
    for (int i = 0; i < memory->OXYGEN_QUEUE.size; i++) {
        atom* atom= pop_atom(&memory->OXYGEN_QUEUE);
        log_(UNUSED, OXYGEN, atom->id, 0);
        free(atom);
    }
    printf("%d\n", memory->HYDROGEN_QUEUE.size);
    for (int i = 0; i < memory->HYDROGEN_QUEUE.size; i++) {
        atom* atom= pop_atom(&memory->HYDROGEN_QUEUE);
        log_(UNUSED, HYDROGEN, atom->id, 0);
        free(atom);
    }
    printf("END \n");

    semaphores_destroy(semaphores);
    free_arguments(args);
    delete_queue(&memory->OXYGEN_QUEUE);
    delete_queue(&memory->HYDROGEN_QUEUE);
    delete_shared_memory(memory);
    return 0;
}

void common_part(arguments* args, sem_t* semaphore, sem_t* sem_ready, atom_queue* queue, element type, bool* ready)
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
    create_atom(id, type);
    atom* atm = create_atom(id, type);
    log_(CREATED, type, id, 0);
    insert_atom(atm, queue);
    if (type == OXYGEN) {
        if (queue->size == args->NO) {
            ready[OXYGEN_READY] = true;
        }
    } else if (type == HYDROGEN) {
        if (queue->size == args->NH) {
            ready[HYDROGEN_READY] = true;
        }
    }
    if (ready[OXYGEN_READY] && ready[HYDROGEN_READY]) {
        sem_post(sem_ready);
    }
    usleep(rand() % args->TI);
    log_(GO_QUEUE, type, id, 0);
    atm->ready = true;
}
