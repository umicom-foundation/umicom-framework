/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_plan.h
 *
 * PURPOSE:
 *   Build ordered application bootstrap stages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_bootstrap_plan_init(UmiBootstrapPlan *plan);
/**
 * Provide the bootstrap plan add stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_plan_add_stage(UmiBootstrapPlan *plan,
                                       const UmiBootstrapStage *stage);

#ifdef __cplusplus
}
#endif

#endif
