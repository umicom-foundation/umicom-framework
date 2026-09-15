/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/bits/main.c
 *
 * PURPOSE:
 *   Display one eight-bit value in decimal, hexadecimal and binary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const unsigned int value = 45U;
    /* C guarantees a byte has at least eight bits, not universally exactly
     * eight. This exercise deliberately prints the low eight value bits. */
    if (printf("Bits in a C byte here: %d\nDecimal: %u\nHexadecimal: %02X\nBinary: ",
               CHAR_BIT, value, value) < 0) return EXIT_FAILURE;
    for (unsigned int position = 8U; position > 0U; --position) {
        const unsigned int bit = (value >> (position - 1U)) & 1U;
        if (putchar(bit != 0U ? '1' : '0') == EOF) return EXIT_FAILURE;
    }
    return putchar('\n') == EOF ? EXIT_FAILURE : EXIT_SUCCESS;
}
