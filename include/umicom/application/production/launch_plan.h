/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/launch_plan.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_LAUNCH_PLAN_H
#define UMICOM_APPLICATION_PRODUCTION_LAUNCH_PLAN_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/lifecycle_gate.h"

typedef struct UmiApplicationProductionLaunchStep {
    UmiApplicationProductionLaunchStage stage;
    UmiApplicationProductionGate gate;
    int required;
    int complete;
} UmiApplicationProductionLaunchStep;

typedef struct UmiApplicationProductionLaunchPlan {
    UmiApplicationProductionLaunchStep
        steps[UMI_APPLICATION_PRODUCTION_MAX_LAUNCH_STEPS];
    size_t count;
    size_t completed_count;
    size_t blocked_count;
} UmiApplicationProductionLaunchPlan;

UmiStatus umi_application_production_launch_plan_build(
    UmiApplicationProductionLaunchPlan *out_plan);
UmiStatus umi_application_production_launch_plan_apply_gate(
    UmiApplicationProductionLaunchPlan *plan,
    const UmiApplicationProductionLifecycleGate *gate);
int umi_application_production_launch_plan_ready(
    const UmiApplicationProductionLaunchPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
