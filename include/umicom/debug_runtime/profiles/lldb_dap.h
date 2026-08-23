/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/lldb_dap.h
 *
 * PURPOSE:
 *   Publish the built-in LLDB DAP Debug Adapter Protocol profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_LLDB_DAP_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_LLDB_DAP_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiDebugAdapterProfile *umi_debug_runtime_profile_lldb_dap(void);
#ifdef __cplusplus
}
#endif
#endif
