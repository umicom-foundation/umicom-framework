/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_plan.h
 *
 * PURPOSE:
 *   ordered package build stages and readiness checking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PLAN_H
#define UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrPackagePlan { char id[UMI_DR_ID_CAPACITY]; uint32_t required_steps; uint32_t completed_steps; bool deterministic; bool privileged; } UmiDrPackagePlan;
void umi_dr_package_plan_init(UmiDrPackagePlan *plan); UmiStatus umi_dr_package_plan_complete_step(UmiDrPackagePlan *plan); bool umi_dr_package_plan_ready(const UmiDrPackagePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
