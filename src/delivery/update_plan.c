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
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise update plan from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_update_plan_init_build_value(UmiUpdatePlan *plan,
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_update_plan_init, UmiUpdatePlan, plan,
    (UmiUpdatePlan *plan, uint64_t current_generation, uint64_t target_generation, UmiUpdateDecision decision, const char *reason),
    umi_update_plan_init_build_value(candidate, current_generation, target_generation, decision, reason))
