/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/lifecycle_plan.h
 *
 * PURPOSE:
 *   Collect and sort lifecycle hooks deterministically by phase and order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_lifecycle_plan_init(UmiBootstrapLifecyclePlan *plan);
UmiStatus umi_bootstrap_lifecycle_plan_add(UmiBootstrapLifecyclePlan *plan,
                                           const UmiBootstrapLifecycleHook *hook);
void umi_bootstrap_lifecycle_plan_sort(UmiBootstrapLifecyclePlan *plan);

#ifdef __cplusplus
}
#endif

#endif
