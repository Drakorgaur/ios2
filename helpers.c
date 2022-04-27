#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

const char* filename = "proj2.out";

int read_arg(char* str)
{
    char* end;
    int i =  (int) strtol (str, &end, 10);
    return i;
}

arguments*  create_arguments()
{
    arguments* args = (arguments*) malloc(sizeof(arguments));

    return args;
}

void free_arguments(arguments* args)
{
    free(args);
}

atom* create_atom(int id, element type)
{
    atom* a = (atom*) malloc(sizeof(atom));
    a->id = id;
    a->type = type;
    a->ready = 0;
    return a;
}

void insert_atom(atom* a, atom_queue* q)
{
    q->atoms[q->size] = *a;
    q->size++;
    q->fake_size++;
}

atom* pop_atom(atom_queue* q)
{
    atom* a = (atom*) malloc(sizeof(atom));
    *a = q->atoms[0];
    q->size--;
    for (int i = 0; i <= q->size; i++)
    {
        q->atoms[i] = q->atoms[i+1];
    }
    return a;
}


void log_(state S, element atom, int atom_id, int molecule_id, int* line_number, sem_t* liner)
{
//    sem_wait(liner);

    *line_number = *line_number + 1;
    FILE* fp = fopen(filename, "a");
    switch (S) {
        case CREATED:
            fprintf(fp, "%d: %c %d: started \n", *line_number, atom, atom_id);
            printf("%d: %c %d: started \n", *line_number, atom, atom_id);
            break;
        case GO_QUEUE:
            fprintf(fp, "%d: %c %d: going to queue \n", *line_number, atom, atom_id);
            printf("%d: %c %d: going to queue \n", *line_number, atom, atom_id);
            break;
        case IN_USE:
            fprintf(fp, "%d: %c %d: creating molecule %d \n", *line_number, atom, atom_id, molecule_id);
            printf("%d: %c %d: creating molecule %d \n", *line_number, atom, atom_id, molecule_id);
            break;
        case USED:
            fprintf(fp, "%d: %c %d: molecule %d created \n", *line_number, atom, atom_id, molecule_id);
            printf("%d: %c %d: molecule %d created \n", *line_number, atom, atom_id, molecule_id);
            break;
        case UNUSED:
            if (atom == 'H') {
                fprintf(fp, "%d: %c %d: not enough O or H \n", *line_number, atom, atom_id);
                printf("%d: %c %d: not enough O or H \n", *line_number, atom, atom_id);
            }
            else {
                fprintf(fp, "%d: %c %d: not enough H \n", *line_number, atom, atom_id);
                printf("%d: %c %d: not enough H \n", *line_number, atom, atom_id);
            }
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
    fclose(fp);
    sem_post(liner);
}