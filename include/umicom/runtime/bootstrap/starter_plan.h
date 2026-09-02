/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_plan.h
 *
 * PURPOSE:
 *   Build deterministic, duplicate-free starter activation plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap starter plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_starter_plan_init(UmiBootstrapStarterPlan *plan);
/**
 * Add bootstrap starter plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_plan_add(
    UmiBootstrapStarterPlan *plan,
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id);

#ifdef __cplusplus
}
#endif

#endif
