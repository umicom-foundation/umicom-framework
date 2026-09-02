/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_plan.c
 *
 * PURPOSE:
 *   Implement the bootstrap plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_plan.c
 *
 * PURPOSE:
 *   Build ordered application bootstrap stages.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_plan.h"


#include <string.h>
/*
 * Initialise bootstrap plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_bootstrap_plan_init(UmiBootstrapPlan *plan) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
/*
 * Provide the bootstrap plan add stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_plan_add_stage(UmiBootstrapPlan *plan,
                                       const UmiBootstrapStage *stage) {
    size_t i, insert_at;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || stage == NULL || !umi_bootstrap_id_valid(stage->stage_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(plan->stages[i].stage_id, stage->stage_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_BOOTSTRAP_MAX_STAGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    insert_at = plan->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i)
        /* Apply this branch only when its contract condition is satisfied. */
        if (stage->order < plan->stages[i].order) { insert_at = i; break; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = plan->count; i > insert_at; --i) plan->stages[i] = plan->stages[i - 1U];
    plan->stages[insert_at] = *stage;
    ++plan->count;
    return UMI_STATUS_OK;
}
