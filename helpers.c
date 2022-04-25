#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

int read_arg(char* str)
{
    char* end;
    return (int) strtol (str, &end, 10);
}

arguments*  create_arguments(char** argv)
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
    switch (S) {
        case CREATED:
            printf("%c %d: process started \n", atom, atom_id);
            break;
        case GO_QUEUE:
            printf("%c %d: going to queue \n", atom, atom_id);
            break;
        case IN_USE:
            printf("%c %d: creating molecule %d \n", atom, atom_id, molecule_id);
            break;
        case USED:
            printf("%c %d: molecule %d created \n", atom, atom_id, molecule_id);
            break;
        case UNUSED:
            if (atom == 'H')
                printf("%c %d: not enough O or H \n", atom, atom_id);
            else
                printf("%c %d:  not enough H \n", atom, atom_id);
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
}