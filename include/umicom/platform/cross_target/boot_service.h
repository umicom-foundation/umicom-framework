/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/boot_service.h
 *
 * PURPOSE:
 *   Bind OS services to boot phases and validate dependency phase ordering.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_BOOT_SERVICE_H
#define UMICOM_PLATFORM_CROSS_TARGET_BOOT_SERVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/boot_phase.h"
typedef struct UmiCtBootService { char service_id[UMI_CT_ID_CAPACITY]; UmiCtBootPhase phase; bool essential; uint32_t timeout_ms; } UmiCtBootService;
UmiStatus umi_ct_boot_service_validate(const UmiCtBootService *service);
bool umi_ct_boot_dependency_phase_valid(const UmiCtBootService *service,const UmiCtBootService *dependency);

#ifdef __cplusplus
}
#endif

#endif
