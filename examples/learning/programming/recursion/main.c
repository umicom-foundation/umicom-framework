/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/recursion/main.c
 *
 * PURPOSE:
 *   Recursion with a bound and base case.
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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static bool Factorial(unsigned value, uint64_t *out)
{
    if (out==NULL || value>20U) return false;
    if (value==0U) { *out=1U; return true; }
    uint64_t smaller=0U;
    if (!Factorial(value-1U,&smaller)) return false;
    *out=smaller*(uint64_t)value; /* 20! fits in uint64_t. */
    return true;
}
int main(void)
{
    uint64_t result=0U;
    if (!Factorial(5U,&result) || result!=120U) return EXIT_FAILURE;
    if (!Factorial(0U,&result) || result!=1U) return EXIT_FAILURE;
    if (!Factorial(20U,&result) || result!=UINT64_C(2432902008176640000)) return EXIT_FAILURE;
    if (Factorial(21U,&result) || result!=UINT64_C(2432902008176640000)) return EXIT_FAILURE;
    printf("20! = %" PRIu64 "; 21 rejected\n",result);
    puts("PASS: recursion");
    return EXIT_SUCCESS;
}
