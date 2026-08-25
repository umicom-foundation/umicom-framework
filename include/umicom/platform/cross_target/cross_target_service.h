/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cross_target_service.h
 *
 * PURPOSE:
 *   Provide the aggregate Framework cross-target readiness service consumed by CLI, Studio, Desk and future Umicom OS tooling.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_SERVICE_H
#define UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_SERVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/target_catalogue.h"
#include "umicom/platform/cross_target/cross_target_snapshot.h"
typedef struct UmiCtCrossTargetService { UmiCtTargetCatalogue targets; UmiCtCrossTargetSnapshot active; bool active_set; uint64_t revision; } UmiCtCrossTargetService;
void umi_ct_cross_target_service_init(UmiCtCrossTargetService *service);
UmiStatus umi_ct_cross_target_service_register(UmiCtCrossTargetService *service,const UmiCtTargetProfile *profile);
UmiStatus umi_ct_cross_target_service_activate(UmiCtCrossTargetService *service,const char *profile_id,const UmiCtCrossTargetSnapshot *snapshot);
const UmiCtCrossTargetSnapshot *umi_ct_cross_target_service_active(const UmiCtCrossTargetService *service);

#ifdef __cplusplus
}
#endif

#endif
