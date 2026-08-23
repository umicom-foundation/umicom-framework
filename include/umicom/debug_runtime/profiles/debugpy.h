/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/debugpy.h
 *
 * PURPOSE:
 *   Publish the built-in Python debugpy Debug Adapter Protocol profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_DEBUGPY_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_DEBUGPY_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiDebugAdapterProfile *umi_debug_runtime_profile_debugpy(void);
#ifdef __cplusplus
}
#endif
#endif
