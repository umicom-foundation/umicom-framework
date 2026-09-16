/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/state_machines/main.c
 *
 * PURPOSE:
 *   State machines and rejected transitions.
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

typedef enum State { DRAFT, CHECKED, PUBLISHED } State;
typedef enum Event { VALIDATE, PUBLISH } Event;
static bool Transition(State *state, Event event)
{
    if (state==NULL) return false;
    if (*state==DRAFT && event==VALIDATE) { *state=CHECKED; return true; }
    if (*state==CHECKED && event==PUBLISH) { *state=PUBLISHED; return true; }
    return false;
}
int main(void)
{
    State state=DRAFT;
    if (Transition(&state,PUBLISH) || state!=DRAFT) return EXIT_FAILURE;
    if (!Transition(&state,VALIDATE) || state!=CHECKED) return EXIT_FAILURE;
    if (!Transition(&state,PUBLISH) || state!=PUBLISHED) return EXIT_FAILURE;
    if (Transition(&state,VALIDATE) || state!=PUBLISHED) return EXIT_FAILURE;
    state=(State)99;
    if (Transition(&state,PUBLISH) || state!=(State)99) return EXIT_FAILURE;
    puts("draft -> checked -> published; illegal transitions rejected");
    puts("PASS: state_machines");
    return EXIT_SUCCESS;
}
