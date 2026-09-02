/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/message.h
 *
 * PURPOSE:
 *   Build and classify Debug Adapter Protocol JSON messages. DAP shares the
 *   Content-Length framing used by LSP but has its own seq/type/command/event
 *   envelope and therefore remains a separate protocol layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_MESSAGE_H
#define UMICOM_DEBUG_RUNTIME_MESSAGE_H

#include "umicom/debug_runtime/types.h"
#include "umicom/language_runtime/json.h"
#include "umicom/language_runtime/json_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the debug runtime build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_build_request(
    uint64_t sequence,
    const char *command,
    const char *arguments_json,
    char *out_json,
    size_t capacity);

/**
 * Provide the debug runtime build response operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_build_response(
    uint64_t sequence,
    uint64_t request_sequence,
    const char *command,
    int success,
    const char *message,
    const char *body_json,
    char *out_json,
    size_t capacity);

/**
 * Read debug runtime message into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_debug_runtime_message_parse(
    const char *json,
    UmiDebugRuntimeEnvelope *out_envelope);

/**
 * Provide the debug runtime message body token operation used by this module and its
 * client applications.
 */
int umi_debug_runtime_message_body_token(
    const UmiLanguageRuntimeJsonDocument *document);

#ifdef __cplusplus
}
#endif
#endif
