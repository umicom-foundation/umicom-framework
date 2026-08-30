/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/rollback.c
 *
 * PURPOSE:
 *   Implement and validate rollback requests between immutable installed generations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Rollback is an explicit operation with a reason and approval state rather than an ad-hoc file copy.
 */

#include "umicom/delivery/rollback.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_rollback_plan_init(UmiRollbackPlan *plan,
                                 uint64_t current_generation,
                                 uint64_t target_generation,
                                 const char *reason)
{
    if (plan == NULL || reason == NULL || target_generation >= current_generation) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    return umi_delivery_copy_text(plan->reason, sizeof(plan->reason), reason);
}

UmiStatus umi_rollback_plan_approve(UmiRollbackPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->approved = 1;
    return UMI_STATUS_OK;
}

int umi_rollback_plan_valid(const UmiRollbackPlan *plan)
{
    return plan != NULL && plan->approved != 0 &&
           plan->target_generation < plan->current_generation;
}
