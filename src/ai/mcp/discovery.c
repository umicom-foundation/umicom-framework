/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/discovery.c
 *
 * PURPOSE:
 *   Implement MCP list-method discovery while storing results in dedicated bounded descriptor catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/discovery.h"

typedef struct ToolVisitorContext {
    UmiAiMcpToolCatalogue *catalogue;
} ToolVisitorContext;

typedef struct ResourceVisitorContext {
    UmiAiMcpResourceCatalogue *catalogue;
} ResourceVisitorContext;

typedef struct PromptVisitorContext {
    UmiAiMcpPromptCatalogue *catalogue;
} PromptVisitorContext;

static UmiStatus visit_tool(
    const char *object_json,
    void *user_data)
{
    ToolVisitorContext *context =
        (ToolVisitorContext *)user_data;
    UmiAiMcpToolDescriptor tool;
    UmiStatus status;

    status = umi_ai_mcp_decode_tool(
        object_json,
        &tool);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_tool_catalogue_add(
        context->catalogue,
        &tool);
}

static UmiStatus visit_resource(
    const char *object_json,
    void *user_data)
{
    ResourceVisitorContext *context =
        (ResourceVisitorContext *)user_data;
    UmiAiMcpResourceDescriptor resource;
    UmiStatus status;

    status = umi_ai_mcp_decode_resource(
        object_json,
        &resource);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_resource_catalogue_add(
        context->catalogue,
        &resource);
}

static UmiStatus visit_prompt(
    const char *object_json,
    void *user_data)
{
    PromptVisitorContext *context =
        (PromptVisitorContext *)user_data;
    UmiAiMcpPromptDescriptor prompt;
    UmiStatus status;

    status = umi_ai_mcp_decode_prompt(
        object_json,
        &prompt);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_prompt_catalogue_add(
        context->catalogue,
        &prompt);
}

static UmiStatus request_list(
    UmiAiMcpSession *session,
    const char *method,
    const char *array_key,
    UmiAiMcpJsonObjectVisitor visitor,
    void *visitor_data)
{
    char result[UMI_AI_MCP_TEXT_CAPACITY];
    size_t count = 0U;
    UmiStatus status;

    if (session == NULL ||
        method == NULL ||
        array_key == NULL ||
        visitor == NULL ||
        session->state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_mcp_transport_request(
        &session->transport,
        method,
        "{}",
        result,
        sizeof(result));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_json_array_visit_objects(
        result,
        array_key,
        visitor,
        visitor_data,
        &count);
}

UmiStatus umi_ai_mcp_discover_tools(
    UmiAiMcpSession *session,
    UmiAiMcpToolCatalogue *catalogue)
{
    ToolVisitorContext context;

    if (catalogue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_mcp_tool_catalogue_init(catalogue);
    context.catalogue = catalogue;

    return request_list(
        session,
        umi_ai_mcp_method_tools_list(),
        "tools",
        visit_tool,
        &context);
}

UmiStatus umi_ai_mcp_discover_resources(
    UmiAiMcpSession *session,
    UmiAiMcpResourceCatalogue *catalogue)
{
    ResourceVisitorContext context;

    if (catalogue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_mcp_resource_catalogue_init(catalogue);
    context.catalogue = catalogue;

    return request_list(
        session,
        umi_ai_mcp_method_resources_list(),
        "resources",
        visit_resource,
        &context);
}

UmiStatus umi_ai_mcp_discover_prompts(
    UmiAiMcpSession *session,
    UmiAiMcpPromptCatalogue *catalogue)
{
    PromptVisitorContext context;

    if (catalogue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_mcp_prompt_catalogue_init(catalogue);
    context.catalogue = catalogue;

    return request_list(
        session,
        umi_ai_mcp_method_prompts_list(),
        "prompts",
        visit_prompt,
        &context);
}
