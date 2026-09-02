/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_plan.h
 *
 * PURPOSE:
 *   Build deterministic ordered service-resolution plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap resolution plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_plan_init(UmiBootstrapResolutionPlan *plan);
/**
 * Add bootstrap resolution plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_resolution_plan_add(UmiBootstrapResolutionPlan *plan,
                                            const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
