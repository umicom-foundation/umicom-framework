/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/pointer_contracts/main.c
 *
 * PURPOSE:
 *   Pointers, borrowing and const.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Borrow values for this call only. Do not free or retain them. */
static bool Minimum(const int *values, size_t count, int *out)
{
    if (values==NULL || count==0U || out==NULL) return false;
    int minimum=values[0];
    for (size_t index=1U; index<count; ++index)
        if (values[index]<minimum) minimum=values[index];
    *out=minimum; return true;
}
int main(void)
{
    int values[]={9,-2,4}; int minimum=99;
    if (!Minimum(values,3U,&minimum) || minimum!=-2) return EXIT_FAILURE;
    if (Minimum(values,0U,&minimum) || minimum!=-2) return EXIT_FAILURE;
    const int *borrowed=values;
    values[0]=8; /* The owner may change a non-const object. */
    if (borrowed[0]!=8 || Minimum(NULL,3U,&minimum)) return EXIT_FAILURE;
    puts("borrowed array minimum=-2; owner retains storage");
    puts("PASS: pointer_contracts");
    return EXIT_SUCCESS;
}
