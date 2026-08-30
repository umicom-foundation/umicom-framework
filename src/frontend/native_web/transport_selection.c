/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/transport_selection.c
 *
 * PURPOSE:
 *   Choose WebSocket, SSE or request/response polling according to negotiated capabilities and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/transport_selection.h"

UmiNativeWebTransportKind umi_native_web_transport_select(const UmiNativeWebBrowserCapability *capability,uint64_t permitted_flags){uint64_t flags;if(capability==NULL)return UMI_NATIVE_WEB_TRANSPORT_NONE;flags=capability->flags&permitted_flags;if((flags&UMI_NATIVE_WEB_CAP_WEBSOCKET)!=0U)return UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET;if((flags&UMI_NATIVE_WEB_CAP_SSE)!=0U)return UMI_NATIVE_WEB_TRANSPORT_SSE;if((flags&UMI_NATIVE_WEB_CAP_POLLING)!=0U)return UMI_NATIVE_WEB_TRANSPORT_POLLING;return UMI_NATIVE_WEB_TRANSPORT_NONE;}

