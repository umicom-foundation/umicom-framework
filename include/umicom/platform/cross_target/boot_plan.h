/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/boot_plan.h
 *
 * PURPOSE:
 *   Produce phase-ordered boot service plans while preserving stable declaration order within each phase.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_BOOT_PLAN_H
#define UMICOM_PLATFORM_CROSS_TARGET_BOOT_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/boot_service.h"
typedef struct UmiCtBootPlan { UmiCtBootService services[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtBootPlan;
void umi_ct_boot_plan_init(UmiCtBootPlan *plan);
UmiStatus umi_ct_boot_plan_add(UmiCtBootPlan *plan,const UmiCtBootService *service);
UmiStatus umi_ct_boot_plan_sort(UmiCtBootPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
