/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_plan.h
 *
 * PURPOSE:
 *   Build ordered application bootstrap stages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_plan_init(UmiBootstrapPlan *plan);
UmiStatus umi_bootstrap_plan_add_stage(UmiBootstrapPlan *plan,
                                       const UmiBootstrapStage *stage);

#ifdef __cplusplus
}
#endif

#endif
