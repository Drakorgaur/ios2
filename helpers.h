#include <semaphore.h>
#include "stdbool.h"

typedef enum element {
    OXYGEN='O',   // 79
    HYDROGEN='H', // 72
} element;

typedef enum state {
    /**
     * enum used to provide logger information about current state of the processes
     */
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
    bool     active;
} atom;

typedef volatile struct atom_queue {
    volatile int size;
    volatile int fake_size;
    int   shmid;
    atom* atoms;
} atom_queue;

typedef struct shared_memory {
    int   shmid;
    int   line;
    int   molecule_id;
    int   oxygen_created;
    int   hydrogen_created;
    atom_queue OXYGEN_QUEUE;
    atom_queue HYDROGEN_QUEUE;
    bool  ready[2];
    int  molecule_status[3];
} shared_memory;

arguments*  create_arguments();
void        free_arguments(arguments* args);
atom*       create_atom(int id, element type);
atom*       pop_atom(atom_queue* queue);
void        insert_atom(atom* a, atom_queue* q);
