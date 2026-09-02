/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/portable_plan.h
 *
 * PURPOSE:
 *   self-contained portable bundle policy and relocatability validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dr portable plan data shared with callers of this public contract.
 */
typedef struct UmiDrPortablePlan { char id[UMI_DR_ID_CAPACITY]; uint32_t required_steps; uint32_t completed_steps; bool deterministic; bool privileged; } UmiDrPortablePlan;
/**
 * Initialise dr portable plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_portable_plan_init(UmiDrPortablePlan *plan); UmiStatus umi_dr_portable_plan_complete_step(UmiDrPortablePlan *plan); bool umi_dr_portable_plan_ready(const UmiDrPortablePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
