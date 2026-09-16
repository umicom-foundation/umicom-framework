/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/boolean_masks/main.c
 *
 * PURPOSE:
 *   Boolean decisions and bit masks.
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

enum { CAN_READ = 1U, CAN_WRITE = 2U, CAN_EXPORT = 4U };
static bool HasAll(uint32_t flags, uint32_t required)
{
    return (flags & required) == required;
}
int main(void)
{
    uint32_t flags = CAN_READ | CAN_EXPORT;
    if (!HasAll(flags,CAN_READ) || HasAll(flags,CAN_WRITE)) return EXIT_FAILURE;
    if (HasAll(flags,CAN_READ | CAN_WRITE)) return EXIT_FAILURE;
    /* Logical && only sees two non-zero values: it cannot test the mask. */
    bool misleading = (flags != 0U) && ((uint32_t)CAN_WRITE != 0U);
    if (!misleading) return EXIT_FAILURE;
    flags |= CAN_WRITE;
    if (!HasAll(flags,CAN_READ | CAN_WRITE)) return EXIT_FAILURE;
    flags &= ~(uint32_t)CAN_EXPORT;
    if (HasAll(flags,CAN_EXPORT)) return EXIT_FAILURE;
    puts("read/write granted; export removed");
    puts("PASS: boolean_masks");
    return EXIT_SUCCESS;
}
