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
#include <stdlib.h>

typedef struct ToolVisitorContext {
    UmiAiMcpToolCatalogue *catalogue;
} ToolVisitorContext;

typedef struct ResourceVisitorContext {
    UmiAiMcpResourceCatalogue *catalogue;
} ResourceVisitorContext;

typedef struct PromptVisitorContext {
    UmiAiMcpPromptCatalogue *catalogue;
} PromptVisitorContext;

/* Provide the visit tool operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_tool_catalogue_add(
        context->catalogue,
        &tool);
}

/* Provide the visit resource operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_resource_catalogue_add(
        context->catalogue,
        &resource);
}

/* Provide the visit prompt operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_prompt_catalogue_add(
        context->catalogue,
        &prompt);
}

/* Provide the request list operation used by this module and its client applications. */
/* Checked transport stages and admits terminated result text before this high-level operation decodes or publishes it. The former implementation is retained for owner review. */
#if 0
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

#endif
/* Reject replies from a session whose readiness or revision changed while the transport callback was running. The former implementation is retained for owner review. */
#if 0
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        method == NULL ||
        array_key == NULL ||
        visitor == NULL ||
        session->state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = UmiAiMcpTransportRequestChecked(
        &session->transport,
        method,
        "{}",
        result,
        sizeof(result));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

#endif
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        method == NULL ||
        array_key == NULL ||
        visitor == NULL ||
        session->state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    const uint64_t session_revision = session->revision;
    status = UmiAiMcpTransportRequestChecked(
        &session->transport,
        method,
        "{}",
        result,
        sizeof(result));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* A reply belongs to the ready session revision that requested it. */
    if (session->state != UMI_AI_MCP_SESSION_READY || session->revision != session_revision)
        return UMI_STATUS_INVALID_STATE;
    return umi_ai_mcp_json_array_visit_objects(
        result,
        array_key,
        visitor,
        visitor_data,
        &count);
}

/*
 * Provide the ai mcp discover tools operation used by this module and its client
 * applications.
 */
/* Build each discovered catalogue privately so admission, transport and late decode failures retain the previous snapshot. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_discover_tools(
    UmiAiMcpSession *session,
    UmiAiMcpToolCatalogue *catalogue)
{
    ToolVisitorContext context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

#endif
UmiStatus umi_ai_mcp_discover_tools(UmiAiMcpSession *session,
    UmiAiMcpToolCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session == NULL || session->state != UMI_AI_MCP_SESSION_READY)
        return UMI_STATUS_INVALID_STATE;
    UmiAiMcpToolCatalogue *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_mcp_tool_catalogue_init(candidate);
    ToolVisitorContext context = {candidate};
    UmiStatus status = request_list(session, umi_ai_mcp_method_tools_list(),
        "tools", visit_tool, &context);
    if (status == UMI_STATUS_OK) *catalogue = *candidate;
    free(candidate);
    return status;
}
/*
 * Provide the ai mcp discover resources operation used by this module and its client
 * applications.
 */
/* Build each discovered catalogue privately so admission, transport and late decode failures retain the previous snapshot. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_discover_resources(
    UmiAiMcpSession *session,
    UmiAiMcpResourceCatalogue *catalogue)
{
    ResourceVisitorContext context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

#endif
UmiStatus umi_ai_mcp_discover_resources(UmiAiMcpSession *session,
    UmiAiMcpResourceCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session == NULL || session->state != UMI_AI_MCP_SESSION_READY)
        return UMI_STATUS_INVALID_STATE;
    UmiAiMcpResourceCatalogue *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_mcp_resource_catalogue_init(candidate);
    ResourceVisitorContext context = {candidate};
    UmiStatus status = request_list(session, umi_ai_mcp_method_resources_list(),
        "resources", visit_resource, &context);
    if (status == UMI_STATUS_OK) *catalogue = *candidate;
    free(candidate);
    return status;
}
/*
 * Provide the ai mcp discover prompts operation used by this module and its client
 * applications.
 */
/* Build each discovered catalogue privately so admission, transport and late decode failures retain the previous snapshot. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_discover_prompts(
    UmiAiMcpSession *session,
    UmiAiMcpPromptCatalogue *catalogue)
{
    PromptVisitorContext context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

#endif
UmiStatus umi_ai_mcp_discover_prompts(UmiAiMcpSession *session,
    UmiAiMcpPromptCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session == NULL || session->state != UMI_AI_MCP_SESSION_READY)
        return UMI_STATUS_INVALID_STATE;
    UmiAiMcpPromptCatalogue *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_mcp_prompt_catalogue_init(candidate);
    PromptVisitorContext context = {candidate};
    UmiStatus status = request_list(session, umi_ai_mcp_method_prompts_list(),
        "prompts", visit_prompt, &context);
    if (status == UMI_STATUS_OK) *catalogue = *candidate;
    free(candidate);
    return status;
}