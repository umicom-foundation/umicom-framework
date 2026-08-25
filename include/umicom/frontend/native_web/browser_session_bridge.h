/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/browser_session_bridge.h
 *
 * PURPOSE:
 *   Project the existing generic frontend web-session snapshot into native-web runtime state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_SESSION_BRIDGE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_SESSION_BRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
#include "umicom/frontend/web_session.h"
typedef struct UmiNativeWebBrowserSessionState { char session_id[UMI_NATIVE_WEB_ID_CAPACITY]; char route[UMI_NATIVE_WEB_TEXT_CAPACITY]; uint64_t last_activity; bool connected; bool suspended; uint64_t revision; } UmiNativeWebBrowserSessionState;
/* Copy canonical frontend session state into the native-web presentation runtime. */
UmiStatus umi_native_web_browser_session_bridge(const UmiFrontendSessionSnapshot *snapshot, UmiNativeWebBrowserSessionState *out_state);

#ifdef __cplusplus
}
#endif
#endif
