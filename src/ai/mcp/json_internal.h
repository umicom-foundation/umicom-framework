/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/json_internal.h
 *
 * PURPOSE:
 *   Share complete bounded MCP object parsing between codecs and list visitors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_MCP_JSON_INTERNAL_H
#define UMICOM_AI_MCP_JSON_INTERNAL_H
#include "umicom/language_runtime/json_tree.h"

/* Internal implementation contract, not an installed public API. On success
 * the caller owns the tree. On failure out_tree is unchanged. */
UmiStatus umi_ai_mcp_json_object_parse(const char *json, UmiJsonTree **out_tree);
#endif
