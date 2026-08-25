/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/capability_boundary.h
 *
 * PURPOSE:
 *   Model capability-based kernel/service admission independently of application-specific permissions.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CAPABILITY_BOUNDARY_H
#define UMICOM_PLATFORM_CROSS_TARGET_CAPABILITY_BOUNDARY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t UmiCtKernelCapability;
enum { UMI_CT_CAP_MEMORY=UINT64_C(1)<<0, UMI_CT_CAP_PROCESS=UINT64_C(1)<<1, UMI_CT_CAP_FILESYSTEM=UINT64_C(1)<<2, UMI_CT_CAP_NETWORK=UINT64_C(1)<<3, UMI_CT_CAP_DEVICE=UINT64_C(1)<<4, UMI_CT_CAP_DEBUG=UINT64_C(1)<<5, UMI_CT_CAP_ADMIN=UINT64_C(1)<<6 };
typedef struct UmiCtCapabilityBoundary { char boundary_id[UMI_CT_ID_CAPACITY]; UmiCtKernelCapability required; UmiCtKernelCapability denied; } UmiCtCapabilityBoundary;
bool umi_ct_capability_boundary_allows(const UmiCtCapabilityBoundary *boundary,UmiCtKernelCapability granted);

#ifdef __cplusplus
}
#endif

#endif
