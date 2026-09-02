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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the ct cross target service data shared with callers of this public contract.
 */
typedef struct UmiCtCrossTargetService { UmiCtTargetCatalogue targets; UmiCtCrossTargetSnapshot active; bool active_set; uint64_t revision; } UmiCtCrossTargetService;
/**
 * Initialise ct cross target service from caller-provided values so later operations
 * receive a known state.
 */
void umi_ct_cross_target_service_init(UmiCtCrossTargetService *service);
/**
 * Add ct cross target service only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ct_cross_target_service_register(UmiCtCrossTargetService *service,const UmiCtTargetProfile *profile);
/**
 * Provide the ct cross target service activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_ct_cross_target_service_activate(UmiCtCrossTargetService *service,const char *profile_id,const UmiCtCrossTargetSnapshot *snapshot);
/**
 * Provide the ct cross target service active operation used by this module and its client
 * applications.
 */
const UmiCtCrossTargetSnapshot *umi_ct_cross_target_service_active(const UmiCtCrossTargetService *service);

#ifdef __cplusplus
}
#endif

#endif
