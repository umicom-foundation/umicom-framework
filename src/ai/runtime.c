/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/runtime.c
 *
 * PURPOSE:
 *   Dispatch AI generation and tool execution through shared registries and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Every generation and tool call passes through one policy-aware dispatch point, simplifying audit and future permissions.
 */

#include "umicom/ai/runtime.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai runtime from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_runtime_init(UmiAiRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) {
        umi_ai_provider_registry_init(&runtime->providers);
        umi_ai_tool_registry_init(&runtime->tools);
        runtime->policy = umi_ai_policy_default();
    }
}

/* Release or reset state held by ai runtime so the same storage can be reused safely. */
void umi_ai_runtime_destroy(UmiAiRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    /* Provider instances are owned by the registry after successful add. */
    umi_ai_provider_registry_destroy(&runtime->providers);
    (void)memset(runtime, 0, sizeof(*runtime));
}

/*
 * Provide the ai runtime generate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_generate(UmiAiRuntime *runtime,
                                  const char *provider_id,
                                  int approved,
                                  const UmiAiRequest *request,
                                  UmiAiResponse *response)
{
    UmiAiProvider *provider;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || provider_id == NULL || request == NULL || response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    provider = umi_ai_provider_registry_find(&runtime->providers, provider_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_provider(&runtime->policy, provider->kind, approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return provider->generate(provider->instance, request, response);
}

/*
 * Provide the ai runtime invoke tool operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_invoke_tool(UmiAiRuntime *runtime,
                                     const char *tool_id,
                                     int approved,
                                     const char *arguments_json,
                                     char *output,
                                     size_t output_capacity)
{
    UmiAiTool *tool;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || tool_id == NULL || arguments_json == NULL ||
        output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    tool = umi_ai_tool_registry_find(&runtime->tools, tool_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_tool(&runtime->policy, approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return tool->invoke(arguments_json, output, output_capacity, tool->user_data);
}
