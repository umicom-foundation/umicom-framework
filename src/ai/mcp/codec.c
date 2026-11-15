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
#include "json_internal.h"
#include "umicom/language_runtime/json_tree.h"

#include <stdio.h>
#include <string.h>

/* Provide the optional string operation used by this module and its client applications. */
/* Optional field failures now propagate through structured descriptor admission instead of becoming empty text. The former implementation is retained for owner review. */
#if 0
static void optional_string(
    const char *json,
    const char *key,
    char *output,
    size_t capacity)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_json_string(
            json,
            key,
            output,
            capacity) != UMI_STATUS_OK) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) {
            output[0] = '\0';
        }
    }
}

#endif

/* Provide the optional raw operation used by this module and its client applications. */
/* Optional field failures now propagate through structured descriptor admission instead of becoming empty text. The former implementation is retained for owner review. */
#if 0
static void optional_raw(
    const char *json,
    const char *key,
    char *output,
    size_t capacity)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_json_raw(
            json,
            key,
            output,
            capacity) != UMI_STATUS_OK) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) {
            output[0] = '\0';
        }
    }
}

#endif

/*
 * Provide the ai mcp decode initialize result operation used by this module and its client
 * applications.
 */
/* Initialization now uses the shared owned JSON parser and publishes complete bounded metadata only after field admission. The prior permissive decoder is retained for owner review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_initialize_result(
    const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities)
{
    char server_info[UMI_AI_MCP_TEXT_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result_json == NULL || out_capabilities == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_capabilities, 0, sizeof(*out_capabilities));

    optional_string(
        result_json,
        "protocolVersion",
        out_capabilities->protocol_version,
        sizeof(out_capabilities->protocol_version));

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_mcp_json_has_key(result_json, "tools")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_TOOLS;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_mcp_json_has_key(result_json, "resources")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_RESOURCES;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_mcp_json_has_key(result_json, "prompts")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_PROMPTS;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_mcp_json_has_key(result_json, "logging")) {
        out_capabilities->flags |= UMI_AI_MCP_CAPABILITY_LOGGING;
    }

    /* Apply this branch only when its contract condition is satisfied. */
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

#endif
/* Read interpreted members from their actual object. Missing required fields
 * are malformed replies; duplicates retain the parser's explicit ambiguity status. */
static UmiStatus InitializeMember(const UmiJsonTree *tree, int object,
    const char *name, UmiLanguageRuntimeJsonTokenType kind, int *out)
{
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) return status;
    return UmiJsonTreeKind(tree, *out) == kind ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

static UmiStatus InitializeText(const UmiJsonTree *tree, int object,
    const char *name, char *out, size_t capacity, int require_nonempty)
{
    int node = -1;
    UmiStatus status = InitializeMember(tree, object, name, UMI_LANGUAGE_RUNTIME_JSON_STRING, &node);
    if (status != UMI_STATUS_OK) return status;
    status = UmiJsonTreeText(tree, node, out, capacity);
    if (status == UMI_STATUS_OK && require_nonempty && out[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    return status;
}

/* Initialization and descriptor decoding now share the same bounded object parser without changing negotiated-field admission. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_initialize_result(const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities)
{
    if (result_json == NULL || out_capabilities == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_MCP_TEXT_CAPACITY && result_json[length] != '\0') ++length;
    if (length == UMI_AI_MCP_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_AI_MCP_TEXT_CAPACITY - 1U, UMI_AI_MCP_TEXT_CAPACITY, 128U};
    UmiStatus status = UmiJsonTreeCreate(result_json, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpServerCapabilities candidate = {0};
    int capabilities = -1, server = -1;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, 0, "protocolVersion", candidate.protocol_version,
            sizeof(candidate.protocol_version), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "capabilities", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &capabilities);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "serverInfo", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &server);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "name", candidate.server_name, sizeof(candidate.server_name), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "version", candidate.server_version, sizeof(candidate.server_version), 0);
    static const struct { const char *name; uint32_t flag; } fields[] = {
        {"tools", UMI_AI_MCP_CAPABILITY_TOOLS}, {"resources", UMI_AI_MCP_CAPABILITY_RESOURCES},
        {"prompts", UMI_AI_MCP_CAPABILITY_PROMPTS}, {"logging", UMI_AI_MCP_CAPABILITY_LOGGING}
    };
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(fields) / sizeof(fields[0]); ++i) {
        int node = -1;
        status = UmiJsonTreeMember(tree, capabilities, fields[i].name, &node);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; continue; }
        if (status == UMI_STATUS_OK) {
            if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) status = UMI_STATUS_PARSE_ERROR;
            else candidate.flags |= fields[i].flag;
        }
    }
    /* All text has been validated and decoded into private storage. Extensions
     * are parsed for valid JSON but do not grant recognized capabilities. */
    if (status == UMI_STATUS_OK) *out_capabilities = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif
/* All MCP descriptor paths share complete bounded parsing. The tree owns its
 * source bytes so final output may alias the original caller input safely. */
/* Complete object parsing is shared with MCP list visitors through json_internal.h so byte and depth limits have one implementation. The former implementation is retained for owner review. */
#if 0
static UmiStatus DecodeMcpObject(const char *json, UmiJsonTree **out_tree)
{
    size_t length = 0U;
    while (length < UMI_AI_MCP_TEXT_CAPACITY && json[length] != '\0') ++length;
    if (length == UMI_AI_MCP_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_AI_MCP_TEXT_CAPACITY - 1U, UMI_AI_MCP_TEXT_CAPACITY, 128U};
    UmiStatus status = UmiJsonTreeCreate(json, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        UmiJsonTreeDestroy(tree); return UMI_STATUS_PARSE_ERROR;
    }
    *out_tree = tree;
    return UMI_STATUS_OK;
}

#endif

static UmiStatus DescriptorOptionalText(const UmiJsonTree *tree, const char *name,
    char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, 0, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    return UmiJsonTreeText(tree, node, out, capacity);
}

static UmiStatus DescriptorOptionalRaw(const UmiJsonTree *tree, const char *name,
    UmiLanguageRuntimeJsonTokenType kind, char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, 0, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != kind) return UMI_STATUS_PARSE_ERROR;
    const char *source = NULL; size_t length = 0U;
    status = UmiJsonTreeSourceSpan(tree, node, &source, &length);
    if (status != UMI_STATUS_OK) return status;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, source, length); out[length] = '\0';
    return UMI_STATUS_OK;
}

/* The decoder delegates bounded object ownership to the shared MCP parser while retaining its field admission and staged publication contract. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_initialize_result(const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities)
{
    if (result_json == NULL || out_capabilities == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = DecodeMcpObject(result_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpServerCapabilities candidate = {0};
    int capabilities = -1, server = -1;
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, 0, "protocolVersion", candidate.protocol_version,
            sizeof(candidate.protocol_version), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "capabilities", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &capabilities);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "serverInfo", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &server);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "name", candidate.server_name, sizeof(candidate.server_name), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "version", candidate.server_version, sizeof(candidate.server_version), 0);
    static const struct { const char *name; uint32_t flag; } fields[] = {
        {"tools", UMI_AI_MCP_CAPABILITY_TOOLS}, {"resources", UMI_AI_MCP_CAPABILITY_RESOURCES},
        {"prompts", UMI_AI_MCP_CAPABILITY_PROMPTS}, {"logging", UMI_AI_MCP_CAPABILITY_LOGGING}
    };
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(fields) / sizeof(fields[0]); ++i) {
        int node = -1;
        status = UmiJsonTreeMember(tree, capabilities, fields[i].name, &node);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; continue; }
        if (status == UMI_STATUS_OK) {
            if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) status = UMI_STATUS_PARSE_ERROR;
            else candidate.flags |= fields[i].flag;
        }
    }
    /* All text has been validated and decoded into private storage. Extensions
     * are parsed for valid JSON but do not grant recognized capabilities. */
    if (status == UMI_STATUS_OK) *out_capabilities = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif
UmiStatus umi_ai_mcp_decode_initialize_result(const char *result_json,
    UmiAiMcpServerCapabilities *out_capabilities)
{
    if (result_json == NULL || out_capabilities == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = umi_ai_mcp_json_object_parse(result_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpServerCapabilities candidate = {0};
    int capabilities = -1, server = -1;
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, 0, "protocolVersion", candidate.protocol_version,
            sizeof(candidate.protocol_version), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "capabilities", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &capabilities);
    if (status == UMI_STATUS_OK)
        status = InitializeMember(tree, 0, "serverInfo", UMI_LANGUAGE_RUNTIME_JSON_OBJECT, &server);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "name", candidate.server_name, sizeof(candidate.server_name), 1);
    if (status == UMI_STATUS_OK)
        status = InitializeText(tree, server, "version", candidate.server_version, sizeof(candidate.server_version), 0);
    static const struct { const char *name; uint32_t flag; } fields[] = {
        {"tools", UMI_AI_MCP_CAPABILITY_TOOLS}, {"resources", UMI_AI_MCP_CAPABILITY_RESOURCES},
        {"prompts", UMI_AI_MCP_CAPABILITY_PROMPTS}, {"logging", UMI_AI_MCP_CAPABILITY_LOGGING}
    };
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(fields) / sizeof(fields[0]); ++i) {
        int node = -1;
        status = UmiJsonTreeMember(tree, capabilities, fields[i].name, &node);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; continue; }
        if (status == UMI_STATUS_OK) {
            if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) status = UMI_STATUS_PARSE_ERROR;
            else candidate.flags |= fields[i].flag;
        }
    }
    /* All text has been validated and decoded into private storage. Extensions
     * are parsed for valid JSON but do not grant recognized capabilities. */
    if (status == UMI_STATUS_OK) *out_capabilities = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

/*
 * Provide the ai mcp decode tool operation used by this module and its client
 * applications.
 */
/* Decode complete root members into private storage and propagate malformed optional fields before publishing the descriptor. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_tool(
    const char *object_json,
    UmiAiMcpToolDescriptor *out_tool)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (object_json == NULL || out_tool == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_tool, 0, sizeof(*out_tool));

    /* Apply this branch only when its contract condition is satisfied. */
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

#endif
/* The decoder delegates bounded object ownership to the shared MCP parser while retaining its field admission and staged publication contract. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_tool(const char *object_json, UmiAiMcpToolDescriptor *out_tool)
{
    if (object_json == NULL || out_tool == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = DecodeMcpObject(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpToolDescriptor candidate = {0};
    status = InitializeText(tree, 0, "name", candidate.name, sizeof(candidate.name), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalRaw(tree, "inputSchema", UMI_LANGUAGE_RUNTIME_JSON_OBJECT,
            candidate.input_schema, sizeof(candidate.input_schema));
    if (status == UMI_STATUS_OK) *out_tool = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif
UmiStatus umi_ai_mcp_decode_tool(const char *object_json, UmiAiMcpToolDescriptor *out_tool)
{
    if (object_json == NULL || out_tool == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = umi_ai_mcp_json_object_parse(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpToolDescriptor candidate = {0};
    status = InitializeText(tree, 0, "name", candidate.name, sizeof(candidate.name), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalRaw(tree, "inputSchema", UMI_LANGUAGE_RUNTIME_JSON_OBJECT,
            candidate.input_schema, sizeof(candidate.input_schema));
    if (status == UMI_STATUS_OK) *out_tool = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

/*
 * Provide the ai mcp decode resource operation used by this module and its client
 * applications.
 */
/* Decode complete root members into private storage and propagate malformed optional fields before publishing the descriptor. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_resource(
    const char *object_json,
    UmiAiMcpResourceDescriptor *out_resource)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (object_json == NULL || out_resource == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_resource, 0, sizeof(*out_resource));

    /* Apply this branch only when its contract condition is satisfied. */
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

#endif
/* The decoder delegates bounded object ownership to the shared MCP parser while retaining its field admission and staged publication contract. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_resource(const char *object_json, UmiAiMcpResourceDescriptor *out_resource)
{
    if (object_json == NULL || out_resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = DecodeMcpObject(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpResourceDescriptor candidate = {0};
    status = InitializeText(tree, 0, "uri", candidate.uri, sizeof(candidate.uri), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "name", candidate.name, sizeof(candidate.name));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "mimeType", candidate.mime_type, sizeof(candidate.mime_type));
    if (status == UMI_STATUS_OK) *out_resource = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif
UmiStatus umi_ai_mcp_decode_resource(const char *object_json, UmiAiMcpResourceDescriptor *out_resource)
{
    if (object_json == NULL || out_resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = umi_ai_mcp_json_object_parse(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpResourceDescriptor candidate = {0};
    status = InitializeText(tree, 0, "uri", candidate.uri, sizeof(candidate.uri), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "name", candidate.name, sizeof(candidate.name));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "mimeType", candidate.mime_type, sizeof(candidate.mime_type));
    if (status == UMI_STATUS_OK) *out_resource = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

/*
 * Provide the ai mcp decode prompt operation used by this module and its client
 * applications.
 */
/* Decode complete root members into private storage and propagate malformed optional fields before publishing the descriptor. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_prompt(
    const char *object_json,
    UmiAiMcpPromptDescriptor *out_prompt)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (object_json == NULL || out_prompt == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_prompt, 0, sizeof(*out_prompt));

    /* Apply this branch only when its contract condition is satisfied. */
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

#endif
/* The decoder delegates bounded object ownership to the shared MCP parser while retaining its field admission and staged publication contract. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_decode_prompt(const char *object_json, UmiAiMcpPromptDescriptor *out_prompt)
{
    if (object_json == NULL || out_prompt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = DecodeMcpObject(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpPromptDescriptor candidate = {0};
    status = InitializeText(tree, 0, "name", candidate.name, sizeof(candidate.name), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalRaw(tree, "arguments", UMI_LANGUAGE_RUNTIME_JSON_ARRAY,
            candidate.arguments_schema, sizeof(candidate.arguments_schema));
    if (status == UMI_STATUS_OK) *out_prompt = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif
UmiStatus umi_ai_mcp_decode_prompt(const char *object_json, UmiAiMcpPromptDescriptor *out_prompt)
{
    if (object_json == NULL || out_prompt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = umi_ai_mcp_json_object_parse(object_json, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiAiMcpPromptDescriptor candidate = {0};
    status = InitializeText(tree, 0, "name", candidate.name, sizeof(candidate.name), 1);
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalText(tree, "description", candidate.description, sizeof(candidate.description));
    if (status == UMI_STATUS_OK)
        status = DescriptorOptionalRaw(tree, "arguments", UMI_LANGUAGE_RUNTIME_JSON_ARRAY,
            candidate.arguments_schema, sizeof(candidate.arguments_schema));
    if (status == UMI_STATUS_OK) *out_prompt = candidate;
    UmiJsonTreeDestroy(tree);
    return status;
}

/* Provide the encode named raw operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai mcp encode initialize params operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_mcp_json_escape_string(
        client_version,
        escaped_version,
        sizeof(escaped_version));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        out_json,
        output_capacity,
        "{\"protocolVersion\":\"" UMI_AI_MCP_PROTOCOL_VERSION "\","
        "\"capabilities\":{},"
        "\"clientInfo\":{\"name\":%s,\"version\":%s}}",
        escaped_name,
        escaped_version);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai mcp encode tool call params operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_encode_tool_call_params(
    const char *tool_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the ai mcp encode resource read params operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_mcp_encode_resource_read_params(
    const char *uri,
    char *out_json,
    size_t output_capacity)
{
    char escaped_uri[UMI_AI_MCP_TEXT_CAPACITY];
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (uri == NULL || out_json == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_json_escape_string(
        uri,
        escaped_uri,
        sizeof(escaped_uri));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the ai mcp encode prompt get params operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_encode_prompt_get_params(
    const char *prompt_name,
    const char *arguments_json,
    char *out_json,
    size_t output_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
