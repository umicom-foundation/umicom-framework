/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/memory_lifetimes/main.c
 *
 * PURPOSE:
 *   Object lifetime, stack and heap.
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

static unsigned NextVisit(void)
{
    static unsigned visits=0U; /* Static storage lives for the programme. */
    return ++visits;
}
static bool CreateValue(int initial, int **out)
{
    if (out==NULL) return false;
    *out=NULL;
    int *owned=malloc(sizeof(*owned));
    if (owned==NULL) return false;
    *owned=initial; *out=owned; return true;
}
int main(void)
{
    int automatic=7;
    const int *borrowed=&automatic; /* Valid only during automatic's lifetime. */
    int *owned=NULL;
    if (!CreateValue(12,&owned)) return EXIT_FAILURE;
    bool correct=*borrowed==7 && *owned==12 && NextVisit()==1U && NextVisit()==2U;
    free(owned); owned=NULL;
    if (!correct || owned!=NULL) return EXIT_FAILURE;
    puts("borrowed automatic value=7; owned value=12; released once");
    puts("PASS: memory_lifetimes");
    return EXIT_SUCCESS;
}
