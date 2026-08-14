/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/compensation_plan.c
 * PURPOSE: Implement approved reverse-order Helix v2 rollback steps.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/compensation_plan.h"

#include <string.h>

void umi_helix_compensation_plan_init(UmiHelixCompensationPlan *plan)
{
    if (plan != NULL) {
        (void)memset(plan, 0, sizeof(*plan));
        plan->state = UMI_HELIX_ROLLBACK_EMPTY;
    }
}

UmiStatus umi_helix_compensation_plan_add(UmiHelixCompensationPlan *plan,
                                   const UmiHelixAction *compensation)
{
    if (plan == NULL || compensation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->state == UMI_HELIX_ROLLBACK_RUNNING ||
        plan->state == UMI_HELIX_ROLLBACK_COMPLETE) return UMI_STATUS_INVALID_STATE;
    if (plan->count >= UMI_HELIX_RUNTIME_MAX_COMPENSATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    plan->compensations[plan->count++] = *compensation;
    plan->remaining = plan->count;
    plan->state = UMI_HELIX_ROLLBACK_READY;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_compensation_plan_begin(UmiHelixCompensationPlan *plan,
                                     int approved)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (plan->state != UMI_HELIX_ROLLBACK_READY || plan->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->remaining = plan->count;
    plan->state = UMI_HELIX_ROLLBACK_RUNNING;
    return UMI_STATUS_OK;
}

const UmiHelixAction *umi_helix_compensation_plan_next(
    UmiHelixCompensationPlan *plan)
{
    if (plan == NULL || plan->state != UMI_HELIX_ROLLBACK_RUNNING ||
        plan->remaining == 0U) return NULL;
    return &plan->compensations[plan->remaining - 1U];
}

UmiStatus umi_helix_compensation_plan_record(UmiHelixCompensationPlan *plan,
                                      UmiStatus result)
{
    if (plan == NULL || plan->state != UMI_HELIX_ROLLBACK_RUNNING ||
        plan->remaining == 0U) return UMI_STATUS_INVALID_STATE;
    if (result != UMI_STATUS_OK) {
        plan->state = UMI_HELIX_ROLLBACK_FAILED;
        return result;
    }
    plan->remaining--;
    if (plan->remaining == 0U) plan->state = UMI_HELIX_ROLLBACK_COMPLETE;
    return UMI_STATUS_OK;
}
