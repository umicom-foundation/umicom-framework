/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/memory/main.c
 *
 * PURPOSE:
 *   Read an array through a pointer without crossing its bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* The caller supplies both the data and the number of readable elements. */
static int SumValues(const int *values, size_t count, int *outSum)
{
    /* These small fixed exercise values are bounded by the caller. A general
     * integer-sum API would also need an arithmetic-overflow contract. */
    int total = 0;
    if (outSum == NULL || (count != 0U && values == NULL)) {
        return 0;
    }
    for (size_t index = 0U; index < count; ++index) {
        total += values[index];
    }
    *outSum = total;
    return 1;
}

int main(void)
{
    const int values[] = {3, 4, 5};
    int total = 0;
    if (!SumValues(values, sizeof(values) / sizeof(values[0]), &total)) {
        return EXIT_FAILURE;
    }
    if (total != 12 || SumValues(NULL, 1U, &total) != 0 || total != 12) {
        return EXIT_FAILURE;
    }
    if (!SumValues(NULL, 0U, &total) || total != 0) {
        return EXIT_FAILURE;
    }
    (void)puts("Array total: 12. Empty total: 0. Invalid pointer rejected.");
    return EXIT_SUCCESS;
}
