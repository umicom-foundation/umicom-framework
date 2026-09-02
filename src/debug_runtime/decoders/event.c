/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/event.c
 *
 * PURPOSE:
 *   Decode DAP asynchronous event body into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/event.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode event operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_event(
    const char *json,
    UmiDebugRuntimeEvent *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int event_token;
    int body;
    int64_t thread;
    int64_t exit_code;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    event_token = umi_language_runtime_json_object_get(
        &document, 0, "event");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (event_token < 0) return UMI_STATUS_PARSE_ERROR;
    status = umi_language_runtime_json_string(
        &document,
        event_token,
        out_result->event,
        sizeof(out_result->event));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    body = umi_debug_runtime_decoder_event_body_token(&document);
    /* Apply this branch only when its contract condition is satisfied. */
    if (body < 0 || umi_language_runtime_json_is_null(&document, body)) {
        return UMI_STATUS_OK;
    }

    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "reason",
        out_result->reason, sizeof(out_result->reason));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "description",
        out_result->description, sizeof(out_result->description));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "output",
        out_result->text, sizeof(out_result->text));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "category",
        out_result->category, sizeof(out_result->category));

    thread = umi_debug_runtime_decoder_optional_int(
        &document, body, "threadId", 0);
    out_result->thread_id = thread > 0 ? (uint64_t)thread : 0U;
    exit_code = umi_debug_runtime_decoder_optional_int(
        &document, body, "exitCode", 0);
    out_result->exit_code = exit_code > 0 ? (uint64_t)exit_code : 0U;
    out_result->all_threads_stopped =
        umi_debug_runtime_decoder_optional_bool(
            &document, body, "allThreadsStopped", 0);
    out_result->all_threads_continued =
        umi_debug_runtime_decoder_optional_bool(
            &document, body, "allThreadsContinued", 0);
    out_result->terminated =
        strcmp(out_result->event, "terminated") == 0 ||
        strcmp(out_result->event, "exited") == 0;
    return UMI_STATUS_OK;
}
