/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_helix_fitness.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHelixFitness fitness;
    assert(umi_helix_fitness_calculate(&fitness, 0.95, 0.95, 0.8, 0.9, 0.95, 0.85)
           == UMI_STATUS_OK);
    assert(fitness.passed);
    assert(fitness.total >= 0.85);
    return 0;
}
