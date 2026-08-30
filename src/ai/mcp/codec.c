/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/codec.c
 *
 * PURPOSE:
 *   Implement bounded MCP descriptor decoding and request parameter encoding over the focused JSON helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/codec.h"

#include <stdio.h>
#include <string.h>

static void optional_string(
    const char *json,
    const char *key,
    char *output,
    size_t capacity)
{
    if (umi_ai_mcp_json_string(
            json,
            key,
            output,
            capacity) != UMI_STATUS_OK) {
        if (capacity > 0U) {
            output[0] = '\0';
        }
    }
}

static void optional_raw(
    const char *json,
    const char *key,
    char *output,
    size_t capacity)
{
    if (umi_ai_mcp_json_raw(
            json,
            key,
            output,
            capacity) != UMI_STATUS_OK) {
        if (capacity > 0U) {
            output[0] = '\0';
        }
    }
}

UmiStatus umi_ai_mcp_decode_initialize_result(
    const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities)
{
    char server_info[UMI_AI_MCP_TEXT_CAPACITY];

    if (result_json == NULL || out_capabilities == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_capabilities, 0, sizeof(*out_capabilities));

    optional_string(
        result_json,
        "protocolVersion",
        out_capabilities->protocol_version,
        sizeof(out_capabilities->protocol_version));

    if (umi_ai_mcp_json_has_key(result_json, "tools")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_TOOLS;
    }
    if (umi_ai_mcp_json_has_key(result_json, "resources")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_RESOURCES;
    }
    if (umi_ai_mcp_json_has_key(result_json, "prompts")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_PROMPTS;
    }
    if (umi_ai_mcp_json_has_key(result_json, "logging")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_LOGGING;
    }

    if (umi_ai_mcp_json_raw(
            result_json,
            "serverInfo",
            server_info,
            sizeof(server_info)) == UMI_STATUS_OK) {
        optional_string(
            server_info,
            "name",
            out_capabilities->server_name,
            sizeof(out_capabilities->server_name));
        optional_string(
            server_info,
            "version",
            out_capabilities->server_version,
            sizeof(out_capabilities->server_version));
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_decode_tool(
    const char *object_json,
    UmiAiMcpToolDescriptor *out_tool)
{
    if (object_json == NULL || out_tool == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_tool, 0, sizeof(*out_tool));

    if (umi_ai_mcp_json_string(
            object_json,
            "name",
            out_tool->name,
            sizeof(out_tool->name)) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }

    optional_string(
        object_json,
        "description",
        out_tool->description,
        sizeof(out_tool->description));
    optional_raw(
        object_json,
        "inputSchema",
        out_tool->input_schema,
        sizeof(out_tool->input_schema));

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_decode_resource(
    const char *object_json,
    UmiAiMcpResourceDescriptor *out_resource)
{
    if (object_json == NULL || out_resource == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_resource, 0, sizeof(*out_resource));

    if (umi_ai_mcp_json_string(
            object_json,
            "uri",
            out_resource->uri,
            sizeof(out_resource->uri)) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }

    optional_string(
        object_json,
        "name",
        out_resource->name,
        sizeof(out_resource->name));
    optional_string(
        object_json,
        "description",
        out_resource->description,
        sizeof(out_resource->description));
    optional_string(
        object_json,
        "mimeType",
        out_resource->mime_type,
        sizeof(out_resource->mime_type));

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_decode_prompt(
    const char *object_json,
    UmiAiMcpPromptDescriptor *out_prompt)
{
    if (object_json == NULL || out_prompt == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_prompt, 0, sizeof(*out_prompt));

    if (umi_ai_mcp_json_string(
            object_json,
            "name",
            out_prompt->name,
            sizeof(out_prompt->name)) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }

    optional_string(
        object_json,
        "description",
        out_prompt->description,
        sizeof(out_prompt->description));
    optional_raw(
        object_json,
        "arguments",
        out_prompt->arguments_schema,
        sizeof(out_prompt->arguments_schema));

    return UMI_STATUS_OK;
}

static UmiStatus encode_named_raw(
    const char *name_key,
    const char *name,
    const char *raw_key,
    const char *raw_value,
    char *out_json,
    size_t output_capacity)
{
    char escaped_name[UMI_AI_SMALL_TEXT_CAPACITY];
    int written;
    UmiStatus status;

    status = umi_ai_mcp_json_escape_string(
        name,
        escaped_name,
        sizeof(escaped_name));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    written = snprintf(
        out_json,
        output_capacity,
        "{\"%s\":%s,\"%s\":%s}",
        name_key,
        escaped_name,
        raw_key,
        raw_value != NULL && raw_value[0] != '\0'
            ? raw_value
            : "{}");

    if (written < 0 || (size_t)written >= output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_encode_initialize_params(
    const char *client_name,
    const char *client_version,
    char *out_json,
    size_t output_capacity)
{
    char escaped_name[UMI_AI_SMALL_TEXT_CAPACITY];
    char escaped_version[256];
    int written;
    UmiStatus status;

    if (client_name == NULL ||
        client_version == NULL ||
        out_json == NULL ||
        output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_json_escape_string(
        client_name,
        escaped_name,
        sizeof(escaped_name));
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_mcp_json_escape_string(
        client_version,
        escaped_version,
        sizeof(escaped_version));
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        out_json,
        output_capacity,
        "{\"protocolVersion\":\"" UMI_AI_MCP_PROTOCOL_VERSION "\","
        "\"capabilities\":{},"
        "\"clientInfo\":{\"name\":%s,\"version\":%s}}",
        escaped_name,
        escaped_version);

    if (written < 0 || (size_t)written >= output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_encode_tool_call_params(
    const char *tool_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity)
{
    if (tool_name == NULL ||
        arguments_json == NULL ||
        out_json == NULL ||
        output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return encode_named_raw(
        "name",
        tool_name,
        "arguments",
        arguments_json,
        out_json,
        output_capacity);
}

UmiStatus umi_ai_mcp_encode_resource_read_params(
    const char *uri,
    char *out_json,
    size_t output_capacity)
{
    char escaped_uri[UMI_AI_MCP_TEXT_CAPACITY];
    int written;
    UmiStatus status;

    if (uri == NULL || out_json == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_json_escape_string(
        uri,
        escaped_uri,
        sizeof(escaped_uri));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    written = snprintf(
        out_json,
        output_capacity,
        "{\"uri\":%s}",
        escaped_uri);

    return written < 0 || (size_t)written >= output_capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_encode_prompt_get_params(
    const char *prompt_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity)
{
    if (prompt_name == NULL ||
        arguments_json == NULL ||
        out_json == NULL ||
        output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return encode_named_raw(
        "name",
        prompt_name,
        "arguments",
        arguments_json,
        out_json,
        output_capacity);
}
