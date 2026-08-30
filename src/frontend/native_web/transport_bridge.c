/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/transport_bridge.c
 *
 * PURPOSE:
 *   Project the existing generic frontend transport snapshot into native-web transport state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/transport_bridge.h"

#include <string.h>
static UmiNativeWebTransportKind parse_kind(const char *kind){if(kind==NULL)return UMI_NATIVE_WEB_TRANSPORT_NONE;if(strcmp(kind,"websocket")==0)return UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET;if(strcmp(kind,"sse")==0||strcmp(kind,"event-stream")==0)return UMI_NATIVE_WEB_TRANSPORT_SSE;if(strcmp(kind,"polling")==0||strcmp(kind,"http")==0)return UMI_NATIVE_WEB_TRANSPORT_POLLING;return UMI_NATIVE_WEB_TRANSPORT_NONE;}
UmiStatus umi_native_web_transport_bridge(const UmiFrontendTransportSnapshot *snapshot,UmiNativeWebTransportState *out_state){UmiStatus s;if(snapshot==NULL||out_state==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_state,0,sizeof(*out_state));s=umi_native_web_copy_text(out_state->transport_id,sizeof(out_state->transport_id),snapshot->id);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(out_state->endpoint,sizeof(out_state->endpoint),snapshot->endpoint);if(s!=UMI_STATUS_OK)return s;out_state->kind=parse_kind(snapshot->kind);out_state->sent_messages=snapshot->sent_messages;out_state->received_messages=snapshot->received_messages;out_state->connected=snapshot->connected!=0;out_state->fallback_allowed=snapshot->fallback_allowed!=0;return out_state->kind==UMI_NATIVE_WEB_TRANSPORT_NONE?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;}

