/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/codec.h
 *
 * PURPOSE:
 *   Decode standard MCP descriptors and encode bounded request parameters used by the Framework MCP client.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_CODEC_H
#define UMICOM_AI_MCP_CODEC_H

#include "umicom/ai/mcp/json_array.h"
#include "umicom/ai/mcp/protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai mcp decode initialize result operation used by this module and its client
 * applications.
 */
/* Validate a complete result object under the MCP text bound and 128-container
 * depth limit. Require protocolVersion and serverInfo.name as nonempty strings,
 * serverInfo.version as a string, and capabilities as an object. Recognized
 * direct capabilities must be objects. Duplicate interpreted members return
 * ALREADY_EXISTS; missing/wrong kinds or malformed JSON return PARSE_ERROR.
 * Decoded string overflow returns CAPACITY_EXCEEDED. Failure preserves output.
 * Unknown extensions are parsed but not interpreted; unrelated duplicates remain
 * allowed. The session, not this decoder, checks supported protocol versions. */
UmiStatus umi_ai_mcp_decode_initialize_result(
    const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities);

/**
 * Provide the ai mcp decode tool operation used by this module and its client
 * applications.
 */
/* Descriptor decoders validate a complete bounded JSON object and preserve
 * output on every failure. Identity must be a nonempty root string. Existing
 * optional fields may be absent; present text must be a string that fits its
 * inline storage. inputSchema must be an object and arguments an array when
 * present. Their validated JSON spelling is retained without schema evaluation.
 * Duplicate interpreted members return ALREADY_EXISTS; malformed/missing
 * identity or wrong kinds return PARSE_ERROR; overflow returns CAPACITY_EXCEEDED.
 * Unknown extensions are parsed but not interpreted. This is bounded metadata
 * admission, not full MCP schema conformance or permission to invoke a tool. */
UmiStatus umi_ai_mcp_decode_tool(
    const char *object_json,
    UmiAiMcpToolDescriptor *out_tool);

/**
 * Provide the ai mcp decode resource operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_decode_resource(
    const char *object_json,
    UmiAiMcpResourceDescriptor *out_resource);

/**
 * Provide the ai mcp decode prompt operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_decode_prompt(
    const char *object_json,
    UmiAiMcpPromptDescriptor *out_prompt);

/**
 * Provide the ai mcp encode initialize params operation used by this module and its client
 * applications.
 */
/* Parameter encoders preserve caller output on failure and allow input/output
 * overlap through staging. Text uses validated UTF-8 and complete JSON escaping.
 * Existing escaped-field capacities remain: client/tool/prompt names use
 * UMI_AI_SMALL_TEXT_CAPACITY, client version 256 bytes, resource URI
 * UMI_AI_MCP_TEXT_CAPACITY, all including quotes and the terminating byte.
 * Tool/prompt arguments must be a complete bounded JSON object; empty text
 * retains the legacy empty-object shorthand. Original raw spelling is retained.
 * No tool permission, URI semantics or argument-schema evaluation is implied. */
UmiStatus umi_ai_mcp_encode_initialize_params(
    const char *client_name,
    const char *client_version,
    char *out_json,
    size_t output_capacity);

/**
 * Provide the ai mcp encode tool call params operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_encode_tool_call_params(
    const char *tool_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity);

/**
 * Provide the ai mcp encode resource read params operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_mcp_encode_resource_read_params(
    const char *uri,
    char *out_json,
    size_t output_capacity);

/**
 * Provide the ai mcp encode prompt get params operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_encode_prompt_get_params(
    const char *prompt_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity);

#ifdef __cplusplus
}
#endif
#endif
