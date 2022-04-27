#include <semaphore.h>
#include "stdbool.h"

typedef enum element {
    OXYGEN='O',   // 79
    HYDROGEN='H', // 72

} element;

typedef enum state {
    CREATED,
    GO_QUEUE,
    IN_USE,
    USED,
    UNUSED,
} state;

typedef struct arguments {
    int NO;
    int NH;
    int TI;
    int TB;
    int expected;
} arguments;

typedef struct atom {
    int      id;
    element  type;
    bool ready;
} atom;

typedef volatile struct atom_queue {
    volatile int size;
    volatile int fake_size;
    int   shmid;
    atom* atoms;
} atom_queue;

typedef struct shared_memory {
    int   shmid;
    int   size;
    atom_queue OXYGEN_QUEUE;
    atom_queue HYDROGEN_QUEUE;
    bool  ready[2];
    int  molecule_status[3];
} shared_memory;



arguments*  create_arguments();
void        free_arguments(arguments* args);
atom*       create_atom(int id, element type);
void        insert_atom(atom* a, atom_queue* q);
void        free_atom(atom* a);
void        free_atom_queue(atom_queue* q);
void        common_part(arguments* args, sem_t* semaphore, sem_t* ,atom_queue* queue, element type, bool*);
