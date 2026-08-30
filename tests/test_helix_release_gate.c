/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_helix_release_gate.c
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

/* BEGINNER NOTE:
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include "umicom/umicom.h"

int main(void)
{
    UmiHelixFitness fitness;
    UmiHelixPolicy policy = umi_helix_policy_default();
    UmiHelixReleaseGate gate = {4U, 4U, 1, 1};
    assert(umi_helix_fitness_calculate(&fitness, 0.95, 0.95, 0.9, 0.9, 0.95, 0.85)
           == UMI_STATUS_OK);
    policy.allow_release = 1;
    assert(umi_helix_release_gate_check(&gate, &fitness, &policy) == UMI_STATUS_OK);
    gate.rollback_ready = 0;
    assert(umi_helix_release_gate_check(&gate, &fitness, &policy) == UMI_STATUS_INVALID_STATE);
    return 0;
}
