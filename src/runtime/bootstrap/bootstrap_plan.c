/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_plan.c
 *
 * PURPOSE:
 *   Build ordered application bootstrap stages.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_plan.h"


#include <string.h>
void umi_bootstrap_plan_init(UmiBootstrapPlan *plan) {
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
UmiStatus umi_bootstrap_plan_add_stage(UmiBootstrapPlan *plan,
                                       const UmiBootstrapStage *stage) {
    size_t i, insert_at;
    if (plan == NULL || stage == NULL || !umi_bootstrap_id_valid(stage->stage_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < plan->count; ++i)
        if (strcmp(plan->stages[i].stage_id, stage->stage_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (plan->count >= UMI_BOOTSTRAP_MAX_STAGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    insert_at = plan->count;
    for (i = 0U; i < plan->count; ++i)
        if (stage->order < plan->stages[i].order) { insert_at = i; break; }
    for (i = plan->count; i > insert_at; --i) plan->stages[i] = plan->stages[i - 1U];
    plan->stages[insert_at] = *stage;
    ++plan->count;
    return UMI_STATUS_OK;
}
