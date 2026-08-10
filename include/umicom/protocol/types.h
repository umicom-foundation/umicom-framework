/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/types.h
 *
 * PURPOSE:
 *   Define protocol directions, message kinds and fixed limits for JSON-RPC, LSP and DAP services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_TYPES_H
#define UMICOM_PROTOCOL_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROTOCOL_HEADER_CAPACITY 4096U
#define UMI_PROTOCOL_MESSAGE_CAPACITY 65536U
#define UMI_PROTOCOL_METHOD_CAPACITY 256U
#define UMI_PROTOCOL_URI_CAPACITY 2048U
#define UMI_PROTOCOL_LANGUAGE_CAPACITY 96U
#define UMI_PROTOCOL_MAX_QUEUE 256U
#define UMI_PROTOCOL_MAX_DOCUMENTS 256U
#define UMI_PROTOCOL_MAX_BREAKPOINTS 1024U

typedef enum UmiProtocolDirection {
    UMI_PROTOCOL_CLIENT_TO_SERVER = 0,
    UMI_PROTOCOL_SERVER_TO_CLIENT = 1
} UmiProtocolDirection;

typedef enum UmiJsonRpcKind {
    UMI_JSON_RPC_REQUEST = 0,
    UMI_JSON_RPC_RESPONSE = 1,
    UMI_JSON_RPC_NOTIFICATION = 2,
    UMI_JSON_RPC_ERROR = 3
} UmiJsonRpcKind;

typedef enum UmiProtocolClientState {
    UMI_PROTOCOL_CLIENT_CREATED = 0,
    UMI_PROTOCOL_CLIENT_READY = 1,
    UMI_PROTOCOL_CLIENT_STOPPED = 2,
    UMI_PROTOCOL_CLIENT_FAILED = 3
} UmiProtocolClientState;

const char *umi_json_rpc_kind_text(UmiJsonRpcKind kind);
const char *umi_protocol_client_state_text(UmiProtocolClientState state);

#ifdef __cplusplus
}
#endif

#endif
