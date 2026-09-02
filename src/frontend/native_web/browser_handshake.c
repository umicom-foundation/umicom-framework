/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/browser_handshake.c
 *
 * PURPOSE:
 *   Negotiate protocol version, patch format and transport capabilities for a browser session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/browser_handshake.h"

/*
 * Provide the native web browser handshake negotiate operation used by this module and its
 * client applications.
 */
UmiStatus umi_native_web_browser_handshake_negotiate(const UmiNativeWebBrowserCapability *client,uint64_t server_flags,UmiNativeWebBrowserHandshake *out_handshake){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(client==NULL||out_handshake==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_handshake->protocol_version=client->protocol_version<UMI_NATIVE_WEB_API_VERSION?client->protocol_version:UMI_NATIVE_WEB_API_VERSION;out_handshake->negotiated_flags=client->flags&server_flags;out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_NONE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_WEBSOCKET)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_SSE)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_SSE;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_POLLING)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_POLLING;out_handshake->accepted=out_handshake->protocol_version>0U&&out_handshake->transport!=UMI_NATIVE_WEB_TRANSPORT_NONE;return out_handshake->accepted?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;}

