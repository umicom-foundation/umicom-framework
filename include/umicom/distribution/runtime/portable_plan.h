/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/portable_plan.h
 *
 * PURPOSE:
 *   self-contained portable bundle policy and relocatability validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PORTABLE_PLAN_H
#define UMICOM_DISTRIBUTION_RUNTIME_PORTABLE_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrPortablePlan { char id[UMI_DR_ID_CAPACITY]; uint32_t required_steps; uint32_t completed_steps; bool deterministic; bool privileged; } UmiDrPortablePlan;
void umi_dr_portable_plan_init(UmiDrPortablePlan *plan); UmiStatus umi_dr_portable_plan_complete_step(UmiDrPortablePlan *plan); bool umi_dr_portable_plan_ready(const UmiDrPortablePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
