/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/dynamic_memory/main.c
 *
 * PURPOSE:
 *   Allocation, resizing and ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Zero-sized resize is intentionally rejected; caller owns *items. */
static bool Resize(int **items, size_t count)
{
    if (items==NULL || count==0U || count>SIZE_MAX/sizeof(**items)) return false;
    int *replacement=realloc(*items,count*sizeof(**items));
    if (replacement==NULL) return false;
    *items=replacement; return true;
}
int main(void)
{
    int *items=NULL;
    if (!Resize(&items,2U)) return EXIT_FAILURE;
    items[0]=10; items[1]=20;
    if (!Resize(&items,4U)) { free(items); return EXIT_FAILURE; }
    items[2]=30; items[3]=40;
    bool valid=items[0]==10 && items[1]==20 && items[3]==40;
    int *before=items;
    if (Resize(&items,0U) || items!=before) valid=false;
    if (Resize(&items,SIZE_MAX) || items!=before) valid=false;
    free(items); items=NULL;
    if (!valid) return EXIT_FAILURE;
    puts("resized 2 -> 4; old values preserved; invalid sizes rejected");
    puts("PASS: dynamic_memory");
    return EXIT_SUCCESS;
}
