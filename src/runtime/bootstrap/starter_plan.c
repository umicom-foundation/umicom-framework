/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_plan.c
 *
 * PURPOSE:
 *   Implement the starter plan behavior for
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
 * File: src/runtime/bootstrap/starter_plan.c
 *
 * PURPOSE:
 *   Build deterministic, duplicate-free starter activation plans.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_plan.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"


#include <string.h>
/*
 * Initialise bootstrap starter plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_starter_plan_init(UmiBootstrapStarterPlan *plan) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
/*
 * Add bootstrap starter plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_plan_add(
    UmiBootstrapStarterPlan *plan,
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id) {
    const UmiBootstrapStarterDescriptor *starter;
    size_t i, insert_at;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || catalogue == NULL || !umi_bootstrap_id_valid(starter_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    starter = umi_bootstrap_starter_catalogue_find(catalogue, starter_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (starter == NULL || !starter->enabled) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i)
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strcmp(plan->starter_ids[i], starter_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    insert_at = plan->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i) {
        const UmiBootstrapStarterDescriptor *existing =
            umi_bootstrap_starter_catalogue_find(catalogue, plan->starter_ids[i]);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (existing != NULL &&
            (starter->priority > existing->priority ||
             (starter->priority == existing->priority &&
              strcmp(starter_id, existing->starter_id) < 0))) {
            insert_at = i;
            break;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = plan->count; i > insert_at; --i)
        memcpy(plan->starter_ids[i], plan->starter_ids[i - 1U],
               sizeof(plan->starter_ids[i]));
    (void)umi_bootstrap_copy_text(plan->starter_ids[insert_at],
        UMI_BOOTSTRAP_ID_CAPACITY, starter_id);
    ++plan->count;
    return UMI_STATUS_OK;
}
