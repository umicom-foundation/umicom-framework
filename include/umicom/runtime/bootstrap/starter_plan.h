/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_plan.h
 *
 * PURPOSE:
 *   Build deterministic, duplicate-free starter activation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_starter_plan_init(UmiBootstrapStarterPlan *plan);
UmiStatus umi_bootstrap_starter_plan_add(
    UmiBootstrapStarterPlan *plan,
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id);

#ifdef __cplusplus
}
#endif

#endif
