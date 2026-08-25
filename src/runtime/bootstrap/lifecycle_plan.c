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
void umi_bootstrap_lifecycle_plan_init(UmiBootstrapLifecyclePlan *plan) {
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
static int hook_compare(const UmiBootstrapLifecycleHook *a,
                        const UmiBootstrapLifecycleHook *b) {
    int ap = umi_bootstrap_lifecycle_phase_rank(a->phase);
    int bp = umi_bootstrap_lifecycle_phase_rank(b->phase);
    if (ap != bp) return ap < bp ? -1 : 1;
    if (a->order != b->order) return a->order < b->order ? -1 : 1;
    return umi_bootstrap_text_compare(a->hook_id, b->hook_id);
}
UmiStatus umi_bootstrap_lifecycle_plan_add(UmiBootstrapLifecyclePlan *plan,
                                           const UmiBootstrapLifecycleHook *hook) {
    size_t i;
    if (plan == NULL || hook == NULL || !umi_bootstrap_id_valid(hook->hook_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < plan->count; ++i)
        if (umi_bootstrap_text_compare(plan->hooks[i].hook_id, hook->hook_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (plan->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->hooks[plan->count++] = *hook;
    umi_bootstrap_lifecycle_plan_sort(plan);
    return UMI_STATUS_OK;
}
void umi_bootstrap_lifecycle_plan_sort(UmiBootstrapLifecyclePlan *plan) {
    size_t i, j;
    if (plan == NULL) return;
    for (i = 1U; i < plan->count; ++i) {
        UmiBootstrapLifecycleHook value = plan->hooks[i];
        j = i;
        while (j > 0U && hook_compare(&value, &plan->hooks[j - 1U]) < 0) {
            plan->hooks[j] = plan->hooks[j - 1U];
            --j;
        }
        plan->hooks[j] = value;
    }
}
