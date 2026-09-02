/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/release_gate.c
 *
 * PURPOSE:
 *   Enforce Helix release evidence and rollback readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback readiness is checked before promotion because a generation is not safely deployable when there is no known recovery path.
 */

#include "umicom/helix/release_gate.h"
#include <stddef.h>

/*
 * Provide the helix release gate check operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_release_gate_check(const UmiHelixReleaseGate *gate,
                                       const UmiHelixFitness *fitness,
                                       const UmiHelixPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gate == NULL || fitness == NULL || policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (gate->passed_checks < gate->required_checks || !fitness->passed || !gate->rollback_ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_helix_policy_can_promote(policy, fitness->total, gate->human_approved);
}
