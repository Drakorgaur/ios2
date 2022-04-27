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


void log_(state S, element atom, int atom_id, int molecule_id)
{
    FILE* fp = fopen(filename, "a");
    switch (S) {
        case CREATED:
            fprintf(fp, "%c %d: process started \n", atom, atom_id);
            printf("%c %d: process started \n", atom, atom_id);
            break;
        case GO_QUEUE:
            fprintf(fp, "%c %d: process going to queue \n", atom, atom_id);
            printf("%c %d: going to queue \n", atom, atom_id);
            break;
        case IN_USE:
            fprintf(fp, "%c %d: creating molecule %d \n", atom, atom_id, molecule_id);
            printf("%c %d: creating molecule %d \n", atom, atom_id, molecule_id);
            break;
        case USED:
            fprintf(fp, "%c %d: molecule %d created \n", atom, atom_id, molecule_id);
            printf("%c %d: molecule %d created \n", atom, atom_id, molecule_id);
            break;
        case UNUSED:
            if (atom == 'H') {
                fprintf(fp, "%c %d: not enough O or H \n", atom, atom_id);
                printf("%c %d: not enough O or H \n", atom, atom_id);
            }
            else {
                printf("%c %d: not enough H \n", atom, atom_id);
                fprintf(fp, "%c %d: not enough H \n", atom, atom_id);
            }
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
    fclose(fp);
}