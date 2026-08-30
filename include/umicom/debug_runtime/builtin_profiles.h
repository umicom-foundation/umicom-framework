/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/builtin_profiles.h
 *
 * PURPOSE:
 *   Enumerate and register built-in DAP profiles into the existing Debug Service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_BUILTIN_PROFILES_H
#define UMICOM_DEBUG_RUNTIME_BUILTIN_PROFILES_H
#include "umicom/debug/service.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_debug_runtime_builtin_profile_count(void);
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_at(size_t index);
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_find(
    const char *profile_id);
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_for_kind(
    const char *debugger_kind);
UmiStatus umi_debug_runtime_register_builtin_profiles(UmiDebugService *service);
#ifdef __cplusplus
}
#endif
#endif
