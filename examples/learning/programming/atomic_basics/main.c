/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/atomic_basics/main.c
 *
 * PURPOSE:
 *   Atomic operations and concurrency boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /* A single-threaded API demonstration, not a race/stress test. */
    atomic_uint completed;
    atomic_init(&completed,0U);
    unsigned before=atomic_fetch_add_explicit(&completed,1U,memory_order_relaxed);
    if (before!=0U || atomic_load_explicit(&completed,memory_order_relaxed)!=1U)
        return EXIT_FAILURE;
    unsigned expected=0U;
    if (atomic_compare_exchange_strong(&completed,&expected,5U) || expected!=1U)
        return EXIT_FAILURE;
    expected=1U;
    if (!atomic_compare_exchange_strong(&completed,&expected,5U) || atomic_load(&completed)!=5U)
        return EXIT_FAILURE;
    puts("atomic counter changed 0 -> 1 -> 5; no thread-safety claim for other state");
    puts("PASS: atomic_basics");
    return EXIT_SUCCESS;
}
