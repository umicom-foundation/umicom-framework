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
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise release gate from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_release_gate_init_build_value(UmiReleaseGate *gate,
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_release_gate_init, UmiReleaseGate, gate,
    (UmiReleaseGate *gate, const char *gate_id, int required),
    umi_release_gate_init_build_value(candidate, gate_id, required))


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
