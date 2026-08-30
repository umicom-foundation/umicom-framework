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


void umi_bootstrap_resolution_plan_init(UmiBootstrapResolutionPlan *plan);
UmiStatus umi_bootstrap_resolution_plan_add(UmiBootstrapResolutionPlan *plan,
                                            const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
