/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_plan.c
 *
 * PURPOSE:
 *   Implement the resolution plan behavior for
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
 * File: src/runtime/bootstrap/resolution_plan.c
 *
 * PURPOSE:
 *   Build deterministic ordered service-resolution plans.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_plan.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>
/*
 * Initialise bootstrap resolution plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_plan_init(UmiBootstrapResolutionPlan *plan) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) memset(plan, 0, sizeof(*plan));
}
/*
 * Add bootstrap resolution plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_resolution_plan_add(UmiBootstrapResolutionPlan *plan,
                                            const UmiBootstrapServiceKey *key) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || key == NULL || !umi_bootstrap_id_valid(key->service_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_bootstrap_service_key_equal(&plan->keys[i], key))
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->keys[plan->count++] = *key;
    return UMI_STATUS_OK;
}
