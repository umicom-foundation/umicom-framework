/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/binary_arithmetic/main.c
 *
 * PURPOSE:
 *   Binary arithmetic from first principles.
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

/* Inputs must be bits, outputs distinct. Committed only on success. */
static bool FullAdder(unsigned left, unsigned right, unsigned carryIn,
                      unsigned *sum, unsigned *carryOut)
{
    if (left > 1U || right > 1U || carryIn > 1U || sum == NULL || carryOut == NULL || sum == carryOut)
        return false;
    *sum = left ^ right ^ carryIn;
    *carryOut = (left & right) | ((left ^ right) & carryIn);
    return true;
}
int main(void)
{
    /* Add 13 (1101) to 7 (0111), one bit at a time. */
    unsigned result = 0U, carry = 0U;
    for (unsigned position = 0U; position < 4U; ++position) {
        unsigned bit = 0U, nextCarry = 0U;
        if (!FullAdder((13U >> position) & 1U, (7U >> position) & 1U,
                       carry, &bit, &nextCarry)) return EXIT_FAILURE;
        result |= bit << position;
        carry = nextCarry;
    }
    result |= carry << 4U;
    if (result != 20U) return EXIT_FAILURE;
    for (unsigned a = 0U; a < 2U; ++a)
        for (unsigned b = 0U; b < 2U; ++b)
            for (unsigned c = 0U; c < 2U; ++c) {
                unsigned bit = 0U, next = 0U;
                if (!FullAdder(a,b,c,&bit,&next) || bit + 2U*next != a+b+c)
                    return EXIT_FAILURE;
            }
    unsigned unchanged = 9U, other = 9U;
    if (FullAdder(2U,0U,0U,&unchanged,&other) || unchanged != 9U || other != 9U)
        return EXIT_FAILURE;
    puts("13 + 7 = 20; all eight full-adder inputs checked");
    puts("PASS: binary_arithmetic");
    return EXIT_SUCCESS;
}
