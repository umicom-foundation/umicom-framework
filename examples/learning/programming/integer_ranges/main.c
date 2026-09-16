/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/integer_ranges/main.c
 *
 * PURPOSE:
 *   Integer ranges and checked conversions.
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

static bool ToByte(unsigned value, uint8_t *out)
{
    if (out == NULL || value > UINT8_MAX) return false;
    *out = (uint8_t)value;
    return true;
}
int main(void)
{
    uint8_t output = 7U;
    if (!ToByte(255U,&output) || output != 255U) return EXIT_FAILURE;
    if (ToByte(256U,&output) || output != 255U) return EXIT_FAILURE;
    if (ToByte(1U,NULL)) return EXIT_FAILURE;
    /* Unsigned conversion is defined modulo 256 for an 8-bit uint8_t. */
    uint8_t truncated = (uint8_t)300U;
    if (truncated != 44U) return EXIT_FAILURE;
    printf("CHAR_BIT=%d; rejected 256; unchecked 300 becomes %u\n",CHAR_BIT,(unsigned)truncated);
    puts("PASS: integer_ranges");
    return EXIT_SUCCESS;
}
