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
/**
 * Return the number of records represented by debug runtime builtin profile without
 * changing their state.
 */
size_t umi_debug_runtime_builtin_profile_count(void);
/**
 * Find debug runtime builtin profile while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_at(size_t index);
/**
 * Find debug runtime builtin profile while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_find(
    const char *profile_id);
/**
 * Provide the debug runtime builtin profile for kind operation used by this module and its
 * client applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_for_kind(
    const char *debugger_kind);
/**
 * Provide the debug runtime register builtin profiles operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_register_builtin_profiles(UmiDebugService *service);
#ifdef __cplusplus
}
#endif
#endif
