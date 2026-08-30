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
