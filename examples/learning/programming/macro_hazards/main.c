/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/macro_hazards/main.c
 *
 * PURPOSE:
 *   Macros, precedence and repeated evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#define BAD_DOUBLE(value) value + value
static unsigned DoubleValue(unsigned value) { return value+value; }
int main(void)
{
    /* This example has defined behaviour but the result is surprising. */
    unsigned wrong=3U*BAD_DOUBLE(2U);
    unsigned correct=3U*DoubleValue(2U);
    if (wrong!=8U || correct!=12U) return EXIT_FAILURE;
    unsigned next=1U;
    unsigned once=DoubleValue(next++);
    if (once!=2U || next!=2U) return EXIT_FAILURE;
    puts("macro precedence produced 8; function produced 12; argument evaluated once");
    puts("PASS: macro_hazards");
    return EXIT_SUCCESS;
}
