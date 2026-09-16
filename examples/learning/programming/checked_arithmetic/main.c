/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/checked_arithmetic/main.c
 *
 * PURPOSE:
 *   Check arithmetic before overflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool AddSize(size_t left, size_t right, size_t *out)
{
    if (out==NULL || left>SIZE_MAX-right) return false;
    *out=left+right; return true;
}
static bool AddInt(int left, int right, int *out)
{
    if (out==NULL || (right>0 && left>INT_MAX-right) ||
        (right<0 && left<INT_MIN-right)) return false;
    *out=left+right; return true;
}
int main(void)
{
    size_t total=8U;
    if (AddSize(SIZE_MAX,1U,&total) || total!=8U) return EXIT_FAILURE;
    if (!AddSize(SIZE_MAX-1U,1U,&total) || total!=SIZE_MAX) return EXIT_FAILURE;
    int sum=7;
    if (AddInt(INT_MAX,1,&sum) || sum!=7 || AddInt(INT_MIN,-1,&sum)) return EXIT_FAILURE;
    if (!AddInt(-5,9,&sum) || sum!=4 || !AddInt(INT_MIN,0,&sum) || sum!=INT_MIN)
        return EXIT_FAILURE;
    puts("signed and unsigned boundary additions checked before arithmetic");
    puts("PASS: checked_arithmetic");
    return EXIT_SUCCESS;
}
