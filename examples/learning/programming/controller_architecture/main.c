/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/controller_architecture/main.c
 *
 * PURPOSE:
 *   Master Controller and Slave Controllers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum Command { ADD_POINT, CLEAR_POINTS } Command;
typedef struct Model { unsigned points; } Model;
typedef struct SlaveController { Model *model; } SlaveController;
typedef struct MasterController { SlaveController *learning; } MasterController;
static bool SlaveHandle(SlaveController *slave, Command command)
{
    if (slave==NULL || slave->model==NULL) return false;
    switch (command) {
    case ADD_POINT:
        if (slave->model->points>=100U) return false;
        ++slave->model->points; return true;
    case CLEAR_POINTS: slave->model->points=0U; return true;
    default: return false;
    }
}
static bool MasterDispatch(MasterController *master, Command command)
{
    return master!=NULL && SlaveHandle(master->learning,command);
}
int main(void)
{
    Model model={99U}; SlaveController slave={&model}; MasterController master={&slave};
    if (!MasterDispatch(&master,ADD_POINT) || model.points!=100U) return EXIT_FAILURE;
    if (MasterDispatch(&master,ADD_POINT) || model.points!=100U) return EXIT_FAILURE;
    if (MasterDispatch(&master,(Command)99)) return EXIT_FAILURE;
    if (!MasterDispatch(&master,CLEAR_POINTS) || model.points!=0U) return EXIT_FAILURE;
    puts("Master delegated; Slave enforced the 100-point bound");
    puts("PASS: controller_architecture");
    return EXIT_SUCCESS;
}
