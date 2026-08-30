/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/json.h
 *
 * PURPOSE:
 *   Provide small bounded JSON extraction and encoding helpers for MCP protocol fields without introducing a second general JSON library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_JSON_H
#define UMICOM_AI_MCP_JSON_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_mcp_json_string(
    const char *json,
    const char *key,
    char *out_value,
    size_t value_capacity);

UmiStatus umi_ai_mcp_json_raw(
    const char *json,
    const char *key,
    char *out_value,
    size_t value_capacity);

int umi_ai_mcp_json_has_key(
    const char *json,
    const char *key);

UmiStatus umi_ai_mcp_json_escape_string(
    const char *value,
    char *out_json_string,
    size_t output_capacity);

#ifdef __cplusplus
}
#endif
#endif
