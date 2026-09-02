/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/types.c
 *
 * PURPOSE:
 *   Implement stable protocol kind and client-state text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/types.h"

/*
 * Provide the json rpc kind text operation used by this module and its client
 * applications.
 */
const char *umi_json_rpc_kind_text(UmiJsonRpcKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_JSON_RPC_REQUEST: return "request";
        case UMI_JSON_RPC_RESPONSE: return "response";
        case UMI_JSON_RPC_NOTIFICATION: return "notification";
        case UMI_JSON_RPC_ERROR: return "error";
        default: return "unknown";
    }
}

/*
 * Provide the protocol client state text operation used by this module and its client
 * applications.
 */
const char *umi_protocol_client_state_text(UmiProtocolClientState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_PROTOCOL_CLIENT_CREATED: return "created";
        case UMI_PROTOCOL_CLIENT_READY: return "ready";
        case UMI_PROTOCOL_CLIENT_STOPPED: return "stopped";
        case UMI_PROTOCOL_CLIENT_FAILED: return "failed";
        default: return "unknown";
    }
}
