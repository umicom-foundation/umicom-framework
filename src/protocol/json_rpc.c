/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/json_rpc.c
 *
 * PURPOSE:
 *   Implement bounded JSON-RPC envelope generation and classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/json_rpc.h"

#include <stdio.h>
#include <string.h>

#include "umicom/protocol/json.h"

/* Provide the write json operation used by this module and its client applications. */
static UmiStatus write_json(char *out_json,
                            size_t capacity,
                            const char *format,
                            int64_t id,
                            const char *first,
                            const char *second)
{
    int written = snprintf(out_json,
                           capacity,
                           format,
                           (long long)id,
                           first != NULL ? first : "",
                           second != NULL ? second : "");
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the json rpc build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_request(int64_t id,
                                     const char *method,
                                     const char *params_json,
                                     char *out_json,
                                     size_t capacity)
{
    char escaped[UMI_PROTOCOL_METHOD_CAPACITY * 2U];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (method == NULL || out_json == NULL || params_json == NULL ||
        !umi_json_is_object(params_json)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(method, escaped, sizeof(escaped));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return write_json(out_json,
                      capacity,
                      "{\"jsonrpc\":\"2.0\",\"id\":%lld,\"method\":\"%s\","
                      "\"params\":%s}",
                      id,
                      escaped,
                      params_json);
}

/*
 * Provide the json rpc build notification operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_notification(const char *method,
                                          const char *params_json,
                                          char *out_json,
                                          size_t capacity)
{
    char escaped[UMI_PROTOCOL_METHOD_CAPACITY * 2U];
    int written;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (method == NULL || params_json == NULL || out_json == NULL ||
        !umi_json_is_object(params_json)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(method, escaped, sizeof(escaped));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    written = snprintf(out_json,
                       capacity,
                       "{\"jsonrpc\":\"2.0\",\"method\":\"%s\","
                       "\"params\":%s}",
                       escaped,
                       params_json);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the json rpc build response operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_response(int64_t id,
                                      const char *result_json,
                                      char *out_json,
                                      size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result_json == NULL || out_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return write_json(out_json,
                      capacity,
                      "{\"jsonrpc\":\"2.0\",\"id\":%lld,\"result\":%s%s",
                      id,
                      result_json,
                      "}");
}

/*
 * Provide the json rpc build error operation used by this module and its client
 * applications.
 */
UmiStatus umi_json_rpc_build_error(int64_t id,
                                   int error_code,
                                   const char *message,
                                   char *out_json,
                                   size_t capacity)
{
    char escaped[2048];
    int written;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message == NULL || out_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(message, escaped, sizeof(escaped));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    written = snprintf(out_json,
                       capacity,
                       "{\"jsonrpc\":\"2.0\",\"id\":%lld,\"error\":{"
                       "\"code\":%d,\"message\":\"%s\"}}",
                       (long long)id,
                       error_code,
                       escaped);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Read json rpc into validated module state and return a status when input cannot be used. */
UmiStatus umi_json_rpc_parse(const char *json,
                             UmiJsonRpcMessage *out_message)
{
    int64_t id = 0;
    int64_t error_code = 0;
    UmiStatus id_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out_message == NULL ||
        !umi_json_is_object(json)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_message, 0, sizeof(*out_message));
    id_status = umi_json_get_integer(json, "id", &id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (id_status == UMI_STATUS_OK) {
        out_message->id = id;
        out_message->has_id = 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_json_get_string(json,
                            "method",
                            out_message->method,
                            sizeof(out_message->method)) == UMI_STATUS_OK) {
        out_message->kind = out_message->has_id
            ? UMI_JSON_RPC_REQUEST
            : UMI_JSON_RPC_NOTIFICATION;
        (void)umi_json_get_object(json,
                                  "params",
                                  out_message->params,
                                  sizeof(out_message->params));
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_json_get_object(json,
                            "error",
                            out_message->result,
                            sizeof(out_message->result)) == UMI_STATUS_OK) {
        out_message->kind = UMI_JSON_RPC_ERROR;
        (void)umi_json_get_integer(out_message->result,
                                   "code",
                                   &error_code);
        out_message->error_code = (int)error_code;
        (void)umi_json_get_string(out_message->result,
                                  "message",
                                  out_message->error_message,
                                  sizeof(out_message->error_message));
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_json_get_object(json,
                            "result",
                            out_message->result,
                            sizeof(out_message->result)) == UMI_STATUS_OK) {
        out_message->kind = UMI_JSON_RPC_RESPONSE;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}
