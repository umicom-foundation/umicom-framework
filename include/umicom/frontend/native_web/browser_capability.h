/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/browser_capability.h
 *
 * PURPOSE:
 *   Represent browser feature support without coupling Framework code to user-agent strings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_CAPABILITY_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebBrowserCapability { uint32_t protocol_version; uint64_t flags; size_t max_patch_bytes; size_t max_event_bytes; } UmiNativeWebBrowserCapability;
/* Initialise conservative browser capabilities. */
void umi_native_web_browser_capability_init(UmiNativeWebBrowserCapability *capability);
/* Check whether every required capability bit is present. */
bool umi_native_web_browser_capability_supports(const UmiNativeWebBrowserCapability *capability, uint64_t required_flags);

#ifdef __cplusplus
}
#endif
#endif
