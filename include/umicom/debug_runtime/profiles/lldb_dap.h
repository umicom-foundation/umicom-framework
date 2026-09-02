/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/lldb_dap.h
 *
 * PURPOSE:
 *   Publish the built-in LLDB DAP Debug Adapter Protocol profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_LLDB_DAP_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_LLDB_DAP_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime profile lldb dap operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_lldb_dap(void);
#ifdef __cplusplus
}
#endif
#endif
