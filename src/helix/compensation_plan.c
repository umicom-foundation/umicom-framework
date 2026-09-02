/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/compensation_plan.c
 * PURPOSE: Implement approved reverse-order Helix v2 rollback steps.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/compensation_plan.h"

#include <string.h>

/*
 * Initialise helix compensation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_helix_compensation_plan_init(UmiHelixCompensationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) {
        (void)memset(plan, 0, sizeof(*plan));
        plan->state = UMI_HELIX_ROLLBACK_EMPTY;
    }
}

/*
 * Add helix compensation plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_helix_compensation_plan_add(UmiHelixCompensationPlan *plan,
                                   const UmiHelixAction *compensation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || compensation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->state == UMI_HELIX_ROLLBACK_RUNNING ||
        plan->state == UMI_HELIX_ROLLBACK_COMPLETE) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_HELIX_RUNTIME_MAX_COMPENSATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    plan->compensations[plan->count++] = *compensation;
    plan->remaining = plan->count;
    plan->state = UMI_HELIX_ROLLBACK_READY;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix compensation plan begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_compensation_plan_begin(UmiHelixCompensationPlan *plan,
                                     int approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->state != UMI_HELIX_ROLLBACK_READY || plan->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->remaining = plan->count;
    plan->state = UMI_HELIX_ROLLBACK_RUNNING;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix compensation plan next operation used by this module and its client
 * applications.
 */
const UmiHelixAction *umi_helix_compensation_plan_next(
    UmiHelixCompensationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || plan->state != UMI_HELIX_ROLLBACK_RUNNING ||
        plan->remaining == 0U) return NULL;
    return &plan->compensations[plan->remaining - 1U];
}

/*
 * Provide the helix compensation plan record operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_compensation_plan_record(UmiHelixCompensationPlan *plan,
                                      UmiStatus result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || plan->state != UMI_HELIX_ROLLBACK_RUNNING ||
        plan->remaining == 0U) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) {
        plan->state = UMI_HELIX_ROLLBACK_FAILED;
        return result;
    }
    plan->remaining--;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->remaining == 0U) plan->state = UMI_HELIX_ROLLBACK_COMPLETE;
    return UMI_STATUS_OK;
}
