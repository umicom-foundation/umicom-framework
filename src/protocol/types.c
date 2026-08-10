/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/types.c
 *
 * PURPOSE:
 *   Implement stable protocol kind and client-state text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/types.h"

const char *umi_json_rpc_kind_text(UmiJsonRpcKind kind)
{
    switch (kind) {
        case UMI_JSON_RPC_REQUEST: return "request";
        case UMI_JSON_RPC_RESPONSE: return "response";
        case UMI_JSON_RPC_NOTIFICATION: return "notification";
        case UMI_JSON_RPC_ERROR: return "error";
        default: return "unknown";
    }
}

const char *umi_protocol_client_state_text(UmiProtocolClientState state)
{
    switch (state) {
        case UMI_PROTOCOL_CLIENT_CREATED: return "created";
        case UMI_PROTOCOL_CLIENT_READY: return "ready";
        case UMI_PROTOCOL_CLIENT_STOPPED: return "stopped";
        case UMI_PROTOCOL_CLIENT_FAILED: return "failed";
        default: return "unknown";
    }
}
