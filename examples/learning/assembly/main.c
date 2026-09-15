/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/assembly/main.c
 *
 * PURPOSE:
 *   Compare C and Assembly using boundary values and a full small grid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "add.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const uint32_t values[] = {0U, 1U, 2U, 70U, UINT32_MAX, UINT32_MAX - 1U,
                              UINT32_C(0x80000000), UINT32_C(0x7fffffff)};
    const size_t count = sizeof(values) / sizeof(values[0]);
    for (size_t left = 0U; left < count; ++left) {
        for (size_t right = 0U; right < count; ++right) {
            if (AddTwoC(values[left], values[right]) !=
                AddTwoAssembly(values[left], values[right])) {
                (void)fprintf(stderr, "C and Assembly disagree at %zu, %zu\n", left, right);
                return EXIT_FAILURE;
            }
        }
    }
    for (uint32_t left = 0U; left < 128U; ++left) {
        for (uint32_t right = 0U; right < 128U; ++right) {
            if (AddTwoAssembly(left, right) != AddTwoC(left, right)) return EXIT_FAILURE;
        }
    }
    if (printf("2 + 3 = %" PRIu32 "\nC and Assembly agree on 16448 pairs.\n",
               AddTwoAssembly(2U, 3U)) < 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
