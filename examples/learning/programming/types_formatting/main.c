/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/types_formatting/main.c
 *
 * PURPOSE:
 *   C values, types and output formats.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const size_t lessonCount=53U;
    const uint64_t bytes=UINT64_C(4096);
    const bool ready=true;
    const double ratio=3.0/2.0;
    int integerRatio=3/2;
    if (ratio!=1.5 || integerRatio!=1) return EXIT_FAILURE;
    printf("lessons=%zu; bytes=%" PRIu64 "; ready=%s\n",lessonCount,bytes,ready?"yes":"no");
    printf("3 / 2 = %d; 3.0 / 2.0 = %.1f\n",integerRatio,ratio);
    puts("PASS: types_formatting");
    return EXIT_SUCCESS;
}
