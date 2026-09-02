/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/adapter_profile.h
 *
 * PURPOSE:
 *   Publish the public adapter profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_ADAPTER_PROFILE_H
#define UMICOM_DEBUG_ADAPTER_PROFILE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DEBUG_ADAPTER_PROFILE_CAPACITY 64U
/**
 * Represent the debug adapter profile data shared with callers of this public contract.
 */
typedef struct UmiDebugAdapterProfile{char id[128];char display_name[256];char executable[1024];char arguments[2048];char debugger_kind[128];int supports_launch;int supports_attach;int enabled;uint64_t revision;}UmiDebugAdapterProfile;
/**
 * Represent the debug adapter profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugAdapterProfileRegistry UmiDebugAdapterProfileRegistry;
/**
 * Initialise debug adapter profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_adapter_profile_registry_create(UmiDebugAdapterProfileRegistry **out_registry);
/**
 * Release or reset state held by debug adapter profile registry so the same storage can be
 * reused safely.
 */
void umi_debug_adapter_profile_registry_destroy(UmiDebugAdapterProfileRegistry *registry);
/**
 * Provide the debug adapter profile registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_adapter_profile_registry_upsert(UmiDebugAdapterProfileRegistry *registry,const UmiDebugAdapterProfile *profile);
/**
 * Find debug adapter profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_adapter_profile_registry_find(const UmiDebugAdapterProfileRegistry *registry,const char *id,UmiDebugAdapterProfile *out_profile);
/**
 * Find debug adapter profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_adapter_profile_registry_at(const UmiDebugAdapterProfileRegistry *registry,size_t index,UmiDebugAdapterProfile *out_profile);
/**
 * Return the number of records represented by debug adapter profile registry without
 * changing their state.
 */
size_t umi_debug_adapter_profile_registry_count(const UmiDebugAdapterProfileRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
