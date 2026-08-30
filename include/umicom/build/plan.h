/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/plan.h
 *
 * PURPOSE:
 *   Populate the canonical configure -> build -> test/run graph so every
 *   frontend and CI entry point observes the same dependency contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_PLAN_H
#define UMICOM_BUILD_PLAN_H

#include "umicom/build/graph.h"
#include "umicom/build/policy.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_build_plan_populate_standard(
    UmiBuildGraph *graph,
    const UmiBuildExecutionPolicy *policy,
    int include_run);

#ifdef __cplusplus
}
#endif
#endif
