/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/transport_bridge.h
 *
 * PURPOSE:
 *   Project the existing generic frontend transport snapshot into native-web transport state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TRANSPORT_BRIDGE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TRANSPORT_BRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
#include "umicom/frontend/transport.h"
typedef struct UmiNativeWebTransportState { char transport_id[UMI_NATIVE_WEB_ID_CAPACITY]; UmiNativeWebTransportKind kind; char endpoint[UMI_NATIVE_WEB_TEXT_CAPACITY]; uint64_t sent_messages; uint64_t received_messages; bool connected; bool fallback_allowed; } UmiNativeWebTransportState;
/* Copy canonical frontend transport state into native-web transport state. */
UmiStatus umi_native_web_transport_bridge(const UmiFrontendTransportSnapshot *snapshot, UmiNativeWebTransportState *out_state);

#ifdef __cplusplus
}
#endif
#endif
