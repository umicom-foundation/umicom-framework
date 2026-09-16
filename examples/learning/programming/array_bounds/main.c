/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/array_bounds/main.c
 *
 * PURPOSE:
 *   Array boundaries and empty collections.
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

static bool ReadAt(const int *items, size_t count, size_t index, int *out)
{
    if (items==NULL || out==NULL || index>=count) return false;
    *out=items[index]; return true;
}
int main(void)
{
    const int items[]={10,20,30}; int value=99;
    if (!ReadAt(items,3U,2U,&value) || value!=30) return EXIT_FAILURE;
    if (ReadAt(items,3U,3U,&value) || value!=30) return EXIT_FAILURE;
    if (ReadAt(items,0U,0U,&value) || ReadAt(NULL,3U,0U,&value)) return EXIT_FAILURE;
    int total=0;
    for (size_t index=0U; index<3U; ++index) total+=items[index];
    if (total!=60) return EXIT_FAILURE;
    puts("last valid index=2; rejected index=3; sum=60");
    puts("PASS: array_bounds");
    return EXIT_SUCCESS;
}
