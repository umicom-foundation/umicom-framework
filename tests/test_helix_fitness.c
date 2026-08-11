/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_helix_fitness.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include "umicom/umicom.h"

int main(void)
{
    UmiHelixFitness fitness;
    assert(umi_helix_fitness_calculate(&fitness, 0.95, 0.95, 0.8, 0.9, 0.95, 0.85)
           == UMI_STATUS_OK);
    assert(fitness.passed);
    assert(fitness.total >= 0.85);
    return 0;
}
