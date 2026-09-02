/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/java_debug.h
 *
 * PURPOSE:
 *   Publish the built-in Java Debug Adapter Debug Adapter Protocol profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_JAVA_DEBUG_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_JAVA_DEBUG_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime profile java debug operation used by this module and its
 * client applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_java_debug(void);
#ifdef __cplusplus
}
#endif
#endif
