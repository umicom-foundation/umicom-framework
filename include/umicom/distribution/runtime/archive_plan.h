/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/archive_plan.h
 *
 * PURPOSE:
 *   archive format, compression and deterministic-build policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ARCHIVE_PLAN_H
#define UMICOM_DISTRIBUTION_RUNTIME_ARCHIVE_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrArchivePlan { char id[UMI_DR_ID_CAPACITY]; uint32_t required_steps; uint32_t completed_steps; bool deterministic; bool privileged; } UmiDrArchivePlan;
void umi_dr_archive_plan_init(UmiDrArchivePlan *plan); UmiStatus umi_dr_archive_plan_complete_step(UmiDrArchivePlan *plan); bool umi_dr_archive_plan_ready(const UmiDrArchivePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
