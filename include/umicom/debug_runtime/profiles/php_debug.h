/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profiles/php_debug.h
 *
 * PURPOSE:
 *   Publish the built-in PHP Debug Adapter Debug Adapter Protocol profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_PHP_DEBUG_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_PHP_DEBUG_H
#include "umicom/debug/adapter_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiDebugAdapterProfile *umi_debug_runtime_profile_php_debug(void);
#ifdef __cplusplus
}
#endif
#endif
