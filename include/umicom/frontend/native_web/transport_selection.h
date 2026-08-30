/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/transport_selection.h
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

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TRANSPORT_SELECTION_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TRANSPORT_SELECTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/browser_capability.h"
/* Select the most interactive transport permitted by both capability and policy masks. */
UmiNativeWebTransportKind umi_native_web_transport_select(const UmiNativeWebBrowserCapability *capability, uint64_t permitted_flags);

#ifdef __cplusplus
}
#endif
#endif
