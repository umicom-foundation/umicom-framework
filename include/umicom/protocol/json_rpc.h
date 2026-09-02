/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/json_rpc.h
 *
 * PURPOSE:
 *   Build and parse JSON-RPC 2.0 requests, notifications, responses and errors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_JSON_RPC_H
#define UMICOM_PROTOCOL_JSON_RPC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the json rpc message data shared with callers of this public contract.
 */
typedef struct UmiJsonRpcMessage {
    UmiJsonRpcKind kind;
    int64_t id;
    int has_id;
    int error_code;
    char method[UMI_PROTOCOL_METHOD_CAPACITY];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    char result[UMI_PROTOCOL_MESSAGE_CAPACITY];
    char error_message[1024];
} UmiJsonRpcMessage;

/**
 * Provide the json rpc build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_request(int64_t id,
                                     const char *method,
                                     const char *params_json,
                                     char *out_json,
                                     size_t capacity);
/**
 * Provide the json rpc build notification operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_notification(const char *method,
                                          const char *params_json,
                                          char *out_json,
                                          size_t capacity);
/**
 * Provide the json rpc build response operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_response(int64_t id,
                                      const char *result_json,
                                      char *out_json,
                                      size_t capacity);
/**
 * Provide the json rpc build error operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_error(int64_t id,
                                   int error_code,
                                   const char *message,
                                   char *out_json,
                                   size_t capacity);
/**
 * Read json rpc into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_json_rpc_parse(const char *json,
                             UmiJsonRpcMessage *out_message);

#ifdef __cplusplus
}
#endif

#endif
