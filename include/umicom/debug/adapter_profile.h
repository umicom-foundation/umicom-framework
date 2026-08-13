/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/adapter_profile.h
 * PURPOSE: Describe native and DAP debugger adapters independently of Studio.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
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
typedef struct UmiDebugAdapterProfile{char id[128];char display_name[256];char executable[1024];char arguments[2048];char debugger_kind[128];int supports_launch;int supports_attach;int enabled;uint64_t revision;}UmiDebugAdapterProfile;
typedef struct UmiDebugAdapterProfileRegistry UmiDebugAdapterProfileRegistry;
UmiStatus umi_debug_adapter_profile_registry_create(UmiDebugAdapterProfileRegistry **out_registry);
void umi_debug_adapter_profile_registry_destroy(UmiDebugAdapterProfileRegistry *registry);
UmiStatus umi_debug_adapter_profile_registry_upsert(UmiDebugAdapterProfileRegistry *registry,const UmiDebugAdapterProfile *profile);
UmiStatus umi_debug_adapter_profile_registry_find(const UmiDebugAdapterProfileRegistry *registry,const char *id,UmiDebugAdapterProfile *out_profile);
UmiStatus umi_debug_adapter_profile_registry_at(const UmiDebugAdapterProfileRegistry *registry,size_t index,UmiDebugAdapterProfile *out_profile);
size_t umi_debug_adapter_profile_registry_count(const UmiDebugAdapterProfileRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
