/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/launch_plan.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/launch_plan.h"

#include <string.h>

static const UmiApplicationProductionLaunchStep DEFAULT_STEPS[] = {
    { UMI_APPLICATION_PRODUCTION_STAGE_RESOLVE,
      UMI_APPLICATION_PRODUCTION_GATE_CONTRACT, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_VALIDATE,
      UMI_APPLICATION_PRODUCTION_GATE_MANIFEST, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_LOAD_LAYOUT,
      UMI_APPLICATION_PRODUCTION_GATE_MANIFEST, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_PROBE_CAPABILITIES,
      UMI_APPLICATION_PRODUCTION_GATE_CAPABILITY, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_RECOVER_WORKSPACE,
      UMI_APPLICATION_PRODUCTION_GATE_RECOVERY, 0, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_BIND_PANELS,
      UMI_APPLICATION_PRODUCTION_GATE_CAPABILITY, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_BIND_COMMANDS,
      UMI_APPLICATION_PRODUCTION_GATE_CONTRACT, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_ACCEPT,
      UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE, 1, 0 },
    { UMI_APPLICATION_PRODUCTION_STAGE_START,
      UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE, 1, 0 }
};

/*
 * Provide the application production launch plan build operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_production_launch_plan_build(
    UmiApplicationProductionLaunchPlan *out_plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->count = sizeof(DEFAULT_STEPS) / sizeof(DEFAULT_STEPS[0]);
    (void)memcpy(out_plan->steps, DEFAULT_STEPS, sizeof(DEFAULT_STEPS));
    return UMI_STATUS_OK;
}

/*
 * Provide the application production launch plan apply gate operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_launch_plan_apply_gate(
    UmiApplicationProductionLaunchPlan *plan,
    const UmiApplicationProductionLifecycleGate *gate)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || gate == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    plan->completed_count = 0U;
    plan->blocked_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->steps[index].gate == gate->gate)
            plan->steps[index].complete = gate->passed;
        plan->completed_count += (size_t)plan->steps[index].complete;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->steps[index].required && !plan->steps[index].complete)
            plan->blocked_count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application production launch plan ready operation used by this module and
 * its client applications.
 */
int umi_application_production_launch_plan_ready(
    const UmiApplicationProductionLaunchPlan *plan)
{
    return plan != NULL && plan->count > 0U &&
           plan->blocked_count == 0U &&
           plan->completed_count == plan->count;
}

