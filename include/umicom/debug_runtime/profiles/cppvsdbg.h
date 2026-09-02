/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/cppvsdbg.h
 *
 * PURPOSE:
 *   Publish the built-in Visual Studio C++ Debug Adapter Debug Adapter Protocol profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_CPPVSDBG_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_CPPVSDBG_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime profile cppvsdbg operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_cppvsdbg(void);
#ifdef __cplusplus
}
#endif
#endif
