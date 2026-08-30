/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/browser_session_bridge.c
 *
 * PURPOSE:
 *   Project the existing generic frontend web-session snapshot into native-web runtime state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/browser_session_bridge.h"

#include <string.h>
UmiStatus umi_native_web_browser_session_bridge(const UmiFrontendSessionSnapshot *snapshot,UmiNativeWebBrowserSessionState *out_state){UmiStatus s;if(snapshot==NULL||out_state==NULL||snapshot->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_state,0,sizeof(*out_state));s=umi_native_web_copy_text(out_state->session_id,sizeof(out_state->session_id),snapshot->id);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(out_state->route,sizeof(out_state->route),snapshot->route);if(s!=UMI_STATUS_OK)return s;out_state->last_activity=snapshot->last_activity;out_state->connected=snapshot->connected!=0;out_state->suspended=snapshot->suspended!=0;out_state->revision=snapshot->revision;return UMI_STATUS_OK;}

