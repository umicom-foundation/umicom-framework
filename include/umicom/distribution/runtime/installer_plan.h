/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/installer_plan.h
 *
 * PURPOSE:
 *   installer operations with privilege and reboot evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INSTALLER_PLAN_H
#define UMICOM_DISTRIBUTION_RUNTIME_INSTALLER_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr installer plan data shared with callers of this public contract.
 */
typedef struct UmiDrInstallerPlan { char id[UMI_DR_ID_CAPACITY]; uint32_t required_steps; uint32_t completed_steps; bool deterministic; bool privileged; } UmiDrInstallerPlan;
/**
 * Initialise dr installer plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_installer_plan_init(UmiDrInstallerPlan *plan); UmiStatus umi_dr_installer_plan_complete_step(UmiDrInstallerPlan *plan); bool umi_dr_installer_plan_ready(const UmiDrInstallerPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
