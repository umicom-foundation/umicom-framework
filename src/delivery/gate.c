/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/gate.c
 *
 * PURPOSE:
 *   Implement one release gate and its evidence status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A gate turns a release requirement such as tests, signing or smoke checks into explicit machine-readable state.
 */

#include "umicom/delivery/gate.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise release gate from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_release_gate_init(UmiReleaseGate *gate,
                                const char *gate_id,
                                int required)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gate == NULL || gate_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(gate, 0, sizeof(*gate));
    gate->required = required != 0;
    gate->status = UMI_EVIDENCE_UNKNOWN;
    return umi_delivery_copy_text(gate->gate_id, sizeof(gate->gate_id), gate_id);
}

/*
 * Provide the release gate record operation used by this module and its client
 * applications.
 */
UmiStatus umi_release_gate_record(UmiReleaseGate *gate,
                                  UmiEvidenceStatus status,
                                  const char *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gate == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    gate->status = status;
    return umi_delivery_copy_text(gate->evidence, sizeof(gate->evidence), evidence);
}

/*
 * Provide the release gate passed operation used by this module and its client
 * applications.
 */
int umi_release_gate_passed(const UmiReleaseGate *gate)
{
    return gate != NULL && (!gate->required || gate->status == UMI_EVIDENCE_PASS);
}
