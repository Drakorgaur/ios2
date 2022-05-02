#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char* filename = "proj2.out";

int read_arg(char* str)
{
    /**
     * read arguments from the command line
     * and returns them as an integer
     */
    int i = 0;

    if (strcmp(str, "") == 0) {
        fprintf( stderr, "Missing argument\n");
        exit(1);
    }

    for (; str[i] != 0; i++)
    {
        if (!isdigit(str[i])) {
            fprintf( stderr, "Wrong argument\n");
            exit(1);
        }
    }
    return (int)strtol(str, NULL, 10);
}

arguments* create_arguments()
{
    /**
     * creates an arguments struct
     */
    arguments* args = (arguments*) malloc(sizeof(arguments));

    return args;
}

void free_arguments(arguments* args)
{
    /**
     * frees the arguments struct
     */
    free(args);
}

void free_atom_queue(atom_queue* queue) {
    /**
     * frees the atom queue
     */
    for (int i = 0; i < queue->size; i++) {
        free(&queue->atoms[i]);
    }
}

atom* create_atom(int id, element type)
{
    /**
     * creates an atom struct
     */
    atom* a = (atom*) malloc(sizeof(atom));
    a->id = id;
    a->type = type;
    a->active = 1;
    return a;
}

void insert_atom(atom* a, atom_queue* q)
{
    /**
     * inserts an atom into the atom queue
     */
    q->atoms[q->size] = *a;
    q->size++;
    q->fake_size++;
}

atom* pop_atom(atom_queue* q)
{
    /**
     * pops an atom from the atom queue
     */
    atom* a = &q->atoms[q->fake_size - 1];
    return a;
}

void log_(state S, element atom, int atom_id, int molecule_id, int* line_number, sem_t* liner)
{
    /**
     * Prints in the log file
     */
    *line_number = *line_number + 1;
    FILE* fp = fopen(filename, "a");
    switch (S) {
        case CREATED:
            fprintf(fp, "%d: %c %d: started \n", *line_number, atom, atom_id);
            break;
        case GO_QUEUE:
            fprintf(fp, "%d: %c %d: going to queue \n", *line_number, atom, atom_id);
            break;
        case IN_USE:
            fprintf(fp, "%d: %c %d: creating molecule %d \n", *line_number, atom, atom_id, molecule_id);
            break;
        case USED:
            fprintf(fp, "%d: %c %d: molecule %d created \n", *line_number, atom, atom_id, molecule_id);
            break;
        case UNUSED:
            if (atom == 'H') {
                fprintf(fp, "%d: %c %d: not enough O or H \n", *line_number, atom, atom_id);
            }
            else {
                fprintf(fp, "%d: %c %d: not enough H \n", *line_number, atom, atom_id);
            }
            break;
        default:
            fprintf(fp, "UNKNOWN\n");
            break;
    }
    fclose(fp);
    sem_post(liner);
}