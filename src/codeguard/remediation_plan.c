/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/remediation_plan.c
 *
 * PURPOSE:
 *   Implement the remediation plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Quality remediation plan | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/remediation_plan.h"
#include <string.h>
UmiStatus umi_codeguard_remediation_plan_build(const UmiCodeGuardEvidenceStore *evidence,UmiCodeGuardRemediationPlan *out_plan)
{
    UmiCodeGuardEvidenceState state;
    size_t index;
    if (evidence == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plan,0,sizeof(*out_plan));
    for (state = UMI_CODEGUARD_EVIDENCE_FAIL; state >= UMI_CODEGUARD_EVIDENCE_WARNING; state = (UmiCodeGuardEvidenceState)(state - 1)) {
        for (index = 0U; index < evidence->count; ++index) {
            const UmiCodeGuardEvidence *item = &evidence->items[index];
            UmiCodeGuardRemediationAction *action;
            if (item->state != state || item->remediation[0] == '\0') continue;
            if (out_plan->count >= UMI_CODEGUARD_QUALITY_MAX_ACTIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
            action = &out_plan->actions[out_plan->count++];
            (void)umi_codeguard_quality_copy(action->evidence_id,sizeof(action->evidence_id),item->id);
            action->priority = item->state;
            (void)umi_codeguard_quality_copy(action->path,sizeof(action->path),item->path);
            action->line = item->line;
            (void)umi_codeguard_quality_copy(action->instruction,sizeof(action->instruction),item->remediation);
        }
        if (state == UMI_CODEGUARD_EVIDENCE_WARNING) break;
    }
    return UMI_STATUS_OK;
}
const UmiCodeGuardRemediationAction *umi_codeguard_remediation_plan_at(const UmiCodeGuardRemediationPlan *plan,size_t index)
{
    return plan != NULL && index < plan->count ? &plan->actions[index] : NULL;
}
