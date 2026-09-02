/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/lifecycle_plan.c
 *
 * PURPOSE:
 *   Implement the lifecycle plan behavior for
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
 * File: src/runtime/bootstrap/lifecycle_plan.c
 *
 * PURPOSE:
 *   Collect and sort lifecycle hooks deterministically by phase and order.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/lifecycle_plan.h"
#include "umicom/runtime/bootstrap/lifecycle_phase.h"


#include <string.h>
/*
 * Initialise bootstrap lifecycle plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_lifecycle_plan_init(UmiBootstrapLifecyclePlan *plan) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
/* Provide the hook compare operation used by this module and its client applications. */
static int hook_compare(const UmiBootstrapLifecycleHook *a,
                        const UmiBootstrapLifecycleHook *b) {
    int ap = umi_bootstrap_lifecycle_phase_rank(a->phase);
    int bp = umi_bootstrap_lifecycle_phase_rank(b->phase);
    /* Apply this branch only when its contract condition is satisfied. */
    if (ap != bp) return ap < bp ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->order != b->order) return a->order < b->order ? -1 : 1;
    return umi_bootstrap_text_compare(a->hook_id, b->hook_id);
}
/*
 * Add bootstrap lifecycle plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_lifecycle_plan_add(UmiBootstrapLifecyclePlan *plan,
                                           const UmiBootstrapLifecycleHook *hook) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || hook == NULL || !umi_bootstrap_id_valid(hook->hook_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i)
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_bootstrap_text_compare(plan->hooks[i].hook_id, hook->hook_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->hooks[plan->count++] = *hook;
    umi_bootstrap_lifecycle_plan_sort(plan);
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap lifecycle plan sort operation used by this module and its client
 * applications.
 */
void umi_bootstrap_lifecycle_plan_sort(UmiBootstrapLifecyclePlan *plan) {
    size_t i, j;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 1U; i < plan->count; ++i) {
        UmiBootstrapLifecycleHook value = plan->hooks[i];
        j = i;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (j > 0U && hook_compare(&value, &plan->hooks[j - 1U]) < 0) {
            plan->hooks[j] = plan->hooks[j - 1U];
            --j;
        }
        plan->hooks[j] = value;
    }
}
