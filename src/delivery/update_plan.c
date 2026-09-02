/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_plan.c
 *
 * PURPOSE:
 *   Describe one proposed update before any installed files are changed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Planning first allows Studio to show the target generation, restart requirement and reason before applying an update.
 */

#include "umicom/delivery/update_plan.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise update plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_update_plan_init(UmiUpdatePlan *plan,
                               uint64_t current_generation,
                               uint64_t target_generation,
                               UmiUpdateDecision decision,
                               const char *reason)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || reason == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(plan, 0, sizeof(*plan));
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    plan->decision = decision;
    plan->requires_restart = target_generation != current_generation;
    return umi_delivery_copy_text(plan->reason, sizeof(plan->reason), reason);
}
