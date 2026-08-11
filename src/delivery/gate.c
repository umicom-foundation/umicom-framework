/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/gate.c
 *
 * PURPOSE:
 *   Implement one release gate and its evidence status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A gate turns a release requirement such as tests, signing or smoke checks into explicit machine-readable state.
 */

#include "umicom/delivery/gate.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_release_gate_init(UmiReleaseGate *gate,
                                const char *gate_id,
                                int required)
{
    if (gate == NULL || gate_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(gate, 0, sizeof(*gate));
    gate->required = required != 0;
    gate->status = UMI_EVIDENCE_UNKNOWN;
    return umi_delivery_copy_text(gate->gate_id, sizeof(gate->gate_id), gate_id);
}

UmiStatus umi_release_gate_record(UmiReleaseGate *gate,
                                  UmiEvidenceStatus status,
                                  const char *evidence)
{
    if (gate == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    gate->status = status;
    return umi_delivery_copy_text(gate->evidence, sizeof(gate->evidence), evidence);
}

int umi_release_gate_passed(const UmiReleaseGate *gate)
{
    return gate != NULL && (!gate->required || gate->status == UMI_EVIDENCE_PASS);
}
