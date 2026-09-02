/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/provider_bridge.h
 *
 * PURPOSE:
 *   Plan bootstrap provider fallback between Umicc, Clang and GCC without replacing existing provider implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_PROVIDER_BRIDGE_H
#define UMICOM_COMPILER_NATIVE_PROVIDER_BRIDGE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native provider kind values accepted by this public contract.
 */
typedef enum UmiNativeProviderKind { UMI_NC_PROVIDER_UMICC=1, UMI_NC_PROVIDER_CLANG=2, UMI_NC_PROVIDER_GCC=3 } UmiNativeProviderKind;
/**
 * Represent the native provider availability data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeProviderAvailability { bool umicc; bool clang; bool gcc; } UmiNativeProviderAvailability;
/**
 * Represent the native provider plan data shared with callers of this public contract.
 */
typedef struct UmiNativeProviderPlan { UmiNativeProviderKind order[3U]; size_t count; UmiNativeProviderKind selected; } UmiNativeProviderPlan;
/**
 * Provide the nc provider bridge plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_provider_bridge_plan(const UmiNativeProviderAvailability *availability,bool prefer_native,bool self_host_ready,UmiNativeProviderPlan *out_plan);
/**
 * Provide the nc provider kind name operation used by this module and its client
 * applications.
 */
const char *umi_nc_provider_kind_name(UmiNativeProviderKind kind);
#ifdef __cplusplus
}
#endif
#endif
