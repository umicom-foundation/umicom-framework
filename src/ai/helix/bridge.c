/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/bridge.c
 *
 * PURPOSE:
 *   Run one policy-aware model turn and convert validated output into a Helix action proposal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/bridge.h"
#include "umicom/ai/helix/prompt_builder.h"
#include "umicom/ai/helix/proposal_validator.h"
#include "umicom/ai/helix/response_parser.h"
#include "umicom/ai/helix/role_profile.h"
#include "umicom/ai/runtime.h"
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *output, size_t capacity, const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the action is mutating operation used by this module and its client
 * applications.
 */
static int action_is_mutating(UmiHelixActionKind kind)
{
    return kind == UMI_HELIX_ACTION_FILESYSTEM ||
           kind == UMI_HELIX_ACTION_SOURCE_CONTROL;
}

/* Provide the validate request operation used by this module and its client applications. */
static UmiStatus validate_request(const UmiAiHelixBridge *bridge,
                                  const UmiAiHelixAgentRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || request == NULL || bridge->runtime == NULL ||
        request->operation_id[0] == '\0' || request->objective[0] == '\0' ||
        umi_ai_helix_role_profile_find(request->role) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (strlen(request->context) > bridge->config.max_context_bytes) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise ai helix bridge from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_helix_bridge_init(UmiAiHelixBridge *bridge,
                                   UmiAiRuntime *runtime,
                                   const UmiAiHelixBridgeConfig *config)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || runtime == NULL || config == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_helix_bridge_config_validate(config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    bridge->runtime = runtime;
    bridge->config = *config;
    return UMI_STATUS_OK;
}

/*
 * Perform ai helix bridge through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_helix_bridge_run(UmiAiHelixBridge *bridge,
                                  const UmiAiHelixAgentRequest *request,
                                  UmiAiHelixAgentResult *result)
{
    UmiAiRequest ai_request;
    UmiAiResponse ai_response;
    UmiAiHelixParsedResponse parsed;
    char system_prompt[UMI_AI_TEXT_CAPACITY];
    char user_prompt[UMI_AI_TEXT_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_helix_agent_result_init(result);
    status = validate_request(bridge, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_helix_prompt_build_system(request->role,
                                              system_prompt,
                                              sizeof(system_prompt));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_helix_prompt_build_user(request,
                                            user_prompt,
                                            sizeof(user_prompt));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    memset(&ai_request, 0, sizeof(ai_request));
    memset(&ai_response, 0, sizeof(ai_response));
    status = copy_text(ai_request.request_id,
                       sizeof(ai_request.request_id),
                       request->operation_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(ai_request.model_id,
                       sizeof(ai_request.model_id),
                       bridge->config.model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ai_request.message_count = 2U;
    ai_request.max_output_tokens = bridge->config.max_output_tokens;
    ai_request.temperature = bridge->config.temperature;
    ai_request.allow_tools = 0;
    ai_request.messages[0].role = UMI_AI_ROLE_SYSTEM;
    status = copy_text(ai_request.messages[0].name,
                       sizeof(ai_request.messages[0].name),
                       "helix.system");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(ai_request.messages[0].text,
                       sizeof(ai_request.messages[0].text),
                       system_prompt);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ai_request.messages[1].role = UMI_AI_ROLE_USER;
    status = copy_text(ai_request.messages[1].name,
                       sizeof(ai_request.messages[1].name),
                       "helix.operation");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(ai_request.messages[1].text,
                       sizeof(ai_request.messages[1].text),
                       user_prompt);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_runtime_generate(bridge->runtime,
                                     bridge->config.provider_id,
                                     request->provider_approved,
                                     &ai_request,
                                     &ai_response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ai_response.text[sizeof(ai_response.text) - 1U] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (ai_response.finish_reason == UMI_AI_FINISH_CANCELLED) {
        return UMI_STATUS_CANCELLED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ai_response.finish_reason == UMI_AI_FINISH_ERROR) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (ai_response.finish_reason == UMI_AI_FINISH_LENGTH) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_ai_helix_response_parse(ai_response.text,
                                         bridge->config.strict_protocol,
                                         &parsed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_helix_proposal_validate(&bridge->config,
                                            request->role,
                                            &parsed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    result->disposition = parsed.disposition;
    result->role = request->role;
    result->usage = ai_response.usage;
    result->confidence = parsed.confidence;
    status = copy_text(result->rationale,
                       sizeof(result->rationale),
                       parsed.rationale);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(result->raw_response,
                       sizeof(result->raw_response),
                       ai_response.text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed.disposition == UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION) {
        status = copy_text(result->action.action_id,
                           sizeof(result->action.action_id),
                           request->operation_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        result->action.kind = parsed.action_kind;
        status = copy_text(result->action.summary,
                           sizeof(result->action.summary),
                           parsed.summary);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(result->action.target,
                           sizeof(result->action.target),
                           parsed.target);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        result->action.attempt = 0U;
        result->action.maximum_attempts = 1U;
        result->action.mutating = action_is_mutating(parsed.action_kind);
    }
    return UMI_STATUS_OK;
}
