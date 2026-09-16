/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/callbacks/main.c
 *
 * PURPOSE:
 *   Callbacks and dependency injection.
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

typedef bool (*Visit)(void *context, int value);
static bool ForEach(const int *values, size_t count, Visit visit, void *context)
{
    if (visit==NULL || (count!=0U && values==NULL)) return false;
    for (size_t index=0U; index<count; ++index)
        if (!visit(context,values[index])) return false;
    return true;
}
static bool CountPositive(void *context, int value)
{
    if (context==NULL) return false;
    size_t *count=context;
    if (value>0) ++*count;
    return true;
}
int main(void)
{
    const int values[]={-1,0,2,4}; size_t count=0U;
    if (!ForEach(values,4U,CountPositive,&count) || count!=2U) return EXIT_FAILURE;
    if (ForEach(values,4U,NULL,&count) || ForEach(values,4U,CountPositive,NULL))
        return EXIT_FAILURE;
    if (!ForEach(NULL,0U,CountPositive,&count)) return EXIT_FAILURE;
    puts("callback counted two positive values; null callback rejected");
    puts("PASS: callbacks");
    return EXIT_SUCCESS;
}
