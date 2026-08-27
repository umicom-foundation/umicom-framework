/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/gate.c
 *
 * PURPOSE:
 *   Implement deterministic developer capability gate evaluation for release and product-readiness checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/gate.h"
#include <string.h>

UmiStatus umi_developer_acceptance_gate_evaluate(
    const UmiDeveloperAcceptanceGate *gate,
    const UmiDeveloperAcceptanceEvidenceStore *evidence,
    UmiDeveloperAcceptanceGateResult *out_result)
{
    size_t index;
    if (gate == NULL || evidence == NULL || out_result == NULL ||
        gate->gate_id[0] == '\0' ||
        gate->requirement_count > UMI_DEVELOPER_ACCEPTANCE_MAX_REQUIREMENTS)
        return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(out_result, 0, sizeof(*out_result));
    (void)strncpy(out_result->gate_id, gate->gate_id,
                  sizeof(out_result->gate_id) - 1U);

    for (index = 0U; index < gate->requirement_count; ++index) {
        const UmiDeveloperAcceptanceEvidence *item =
            umi_developer_acceptance_evidence_find(
                evidence, gate->required_capability_ids[index]);
        if (item == NULL) {
            out_result->missing_count += 1U;
            continue;
        }
        if (item->state == UMI_DEVELOPER_ACCEPTANCE_PASS)
            out_result->passed_count += 1U;
        else if (item->state == UMI_DEVELOPER_ACCEPTANCE_BLOCKED)
            out_result->blocked_count += 1U;
        else
            out_result->failed_count += 1U;
    }

    out_result->passed =
        out_result->passed_count == gate->requirement_count &&
        out_result->failed_count == 0U &&
        out_result->blocked_count == 0U &&
        out_result->missing_count == 0U;
    return UMI_STATUS_OK;
}
