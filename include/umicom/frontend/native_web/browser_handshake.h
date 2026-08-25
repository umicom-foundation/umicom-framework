/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/browser_handshake.h
 *
 * PURPOSE:
 *   Negotiate protocol version, patch format and transport capabilities for a browser session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_HANDSHAKE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_BROWSER_HANDSHAKE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/browser_capability.h"
typedef struct UmiNativeWebBrowserHandshake { uint32_t protocol_version; UmiNativeWebTransportKind transport; uint64_t negotiated_flags; bool accepted; } UmiNativeWebBrowserHandshake;
/* Negotiate protocol/capabilities against a server-supported mask. */
UmiStatus umi_native_web_browser_handshake_negotiate(const UmiNativeWebBrowserCapability *client, uint64_t server_flags, UmiNativeWebBrowserHandshake *out_handshake);

#ifdef __cplusplus
}
#endif
#endif
