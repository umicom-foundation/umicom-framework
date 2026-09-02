/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/browser_capability.c
 *
 * PURPOSE:
 *   Represent browser feature support without coupling Framework code to user-agent strings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/browser_capability.h"

/*
 * Initialise native web browser capability from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_browser_capability_init(UmiNativeWebBrowserCapability *capability){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(capability==NULL)return;capability->protocol_version=UMI_NATIVE_WEB_API_VERSION;capability->flags=UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_SSE|UMI_NATIVE_WEB_CAP_POLLING|UMI_NATIVE_WEB_CAP_KEYBOARD|UMI_NATIVE_WEB_CAP_POINTER|UMI_NATIVE_WEB_CAP_ACCESSIBILITY|UMI_NATIVE_WEB_CAP_THEME|UMI_NATIVE_WEB_CAP_DENSITY|UMI_NATIVE_WEB_CAP_RESPONSIVE;capability->max_patch_bytes=65536U;capability->max_event_bytes=8192U;}
/*
 * Provide the native web browser capability supports operation used by this module and its
 * client applications.
 */
bool umi_native_web_browser_capability_supports(const UmiNativeWebBrowserCapability *capability,uint64_t required_flags){return capability!=NULL&&(capability->flags&required_flags)==required_flags;}

