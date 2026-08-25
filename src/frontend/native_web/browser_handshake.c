/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/browser_handshake.c
 *
 * PURPOSE:
 *   Negotiate protocol version, patch format and transport capabilities for a browser session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/browser_handshake.h"

UmiStatus umi_native_web_browser_handshake_negotiate(const UmiNativeWebBrowserCapability *client,uint64_t server_flags,UmiNativeWebBrowserHandshake *out_handshake){if(client==NULL||out_handshake==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_handshake->protocol_version=client->protocol_version<UMI_NATIVE_WEB_API_VERSION?client->protocol_version:UMI_NATIVE_WEB_API_VERSION;out_handshake->negotiated_flags=client->flags&server_flags;out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_NONE;if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_WEBSOCKET)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET;else if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_SSE)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_SSE;else if((out_handshake->negotiated_flags&UMI_NATIVE_WEB_CAP_POLLING)!=0U)out_handshake->transport=UMI_NATIVE_WEB_TRANSPORT_POLLING;out_handshake->accepted=out_handshake->protocol_version>0U&&out_handshake->transport!=UMI_NATIVE_WEB_TRANSPORT_NONE;return out_handshake->accepted?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;}

