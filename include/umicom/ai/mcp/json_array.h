/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/json_array.h
 *
 * PURPOSE:
 *   Iterate bounded JSON object arrays returned by MCP list methods while preserving nested schema objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_JSON_ARRAY_H
#define UMICOM_AI_MCP_JSON_ARRAY_H

#include "umicom/ai/mcp/json.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiAiMcpJsonObjectVisitor)(
    const char *object_json,
    void *user_data);

/**
 * Provide the ai mcp json array visit objects operation used by this module and its client
 * applications.
 */
/* Parse one complete bounded JSON object and select its direct array member.
 * The input must terminate within UMI_AI_MCP_TEXT_CAPACITY bytes. The shared
 * parser admits at most 128 container levels and lookup keys of 4096 bytes.
 * Missing members return NOT_FOUND, duplicates ALREADY_EXISTS, and non-array
 * members or non-object elements PARSE_ERROR before any visitor runs.
 * Each object is borrowed, NUL-terminated raw JSON valid only during its call.
 * The source is snapshotted before callbacks; callbacks may change the original
 * input. Keep out_count in separate live writable storage for the full call.
 * On a valid call, out_count is zero unless every visitor succeeds. Callback
 * failures are returned unchanged; arbitrary callback side effects cannot be
 * rolled back. Catalogue discovery supplies its own unpublished candidate. */
UmiStatus umi_ai_mcp_json_array_visit_objects(
    const char *json,
    const char *array_key,
    UmiAiMcpJsonObjectVisitor visitor,
    void *user_data,
    size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
