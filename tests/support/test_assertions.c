/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/test_assertions.c
 * PURPOSE: Verify that Release fixture setup is evaluated exactly once.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef NDEBUG
#error This regression target must supply NDEBUG before the test policy is included.
#endif
#include "assertions.h"
#include <stdio.h>
#ifdef NDEBUG
#error Required fixture setup must not be disabled in this translation unit.
#endif

/* An explicit check outside assert catches a future policy regression even
 * when the compiler removes the assertion itself. Production build flags are
 * unchanged: this executable is deliberately compiled with NDEBUG. */
int main(void)
{
    unsigned evaluations = 0U;
    assert(++evaluations == 1U);
    if (evaluations != 1U) {
        fputs("Required assertion setup did not execute exactly once.\n", stderr);
        return 1;
    }
    puts("Release fixture setup executed exactly once.");
    return 0;
}
