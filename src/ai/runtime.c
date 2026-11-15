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
#include "provider_registry_internal.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai runtime from caller-provided values so later operations receive a known
 * state.
 */
/* Retain all runtime state during provider callbacks instead of clearing tools or policy after a refused registry initialization. The former implementation is retained for owner review. */
#if 0
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

#endif
void umi_ai_runtime_init(UmiAiRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) {
        if (UmiAiProviderRegistryCheckIdle(&runtime->providers) != UMI_STATUS_OK) return;
        umi_ai_provider_registry_init(&runtime->providers);
        umi_ai_tool_registry_init(&runtime->tools);
        runtime->policy = umi_ai_policy_default();
    }
}

/* Release or reset state held by ai runtime so the same storage can be reused safely. */
/* Checked cleanup preserves corrupt registry evidence and owned instances instead of clearing their only references. Shared admission validates current provider storage and policy before exposing a borrowed table. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus UmiAiRuntimeRelease(UmiAiRuntime *runtime)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryRelease(&runtime->providers);
    if (status != UMI_STATUS_OK) return status;
    memset(runtime, 0, sizeof(*runtime));
    return UMI_STATUS_OK;
}

void umi_ai_runtime_destroy(UmiAiRuntime *runtime)
{
    if (runtime != NULL) (void)UmiAiRuntimeRelease(runtime);
}

UmiStatus UmiAiRuntimeAdmitProvider(UmiAiRuntime *runtime, const char *provider_id,
    int approved, UmiAiProvider **out_provider)
{
    if (runtime == NULL || provider_id == NULL || out_provider == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryValidate(&runtime->providers);
    if (status != UMI_STATUS_OK) return status;
    UmiAiProvider *provider = umi_ai_provider_registry_find(&runtime->providers, provider_id);
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_provider(&runtime->policy, provider->kind, approved);
    if (status != UMI_STATUS_OK) return status;
    *out_provider = provider;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai runtime generate operation used by this module and its client
 * applications.
 */
/* Shared bounded request admission prevents malformed public records reaching provider callbacks. Direct response writes are retained for legacy error-output compatibility. The former implementation is retained for owner review. */
#if 0
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
/* Generation delegates provider resolution and current policy to the shared admission service. The former inline dispatch checks are preserved for comparison. The former implementation is retained for owner review. */
#if 0
    provider = umi_ai_provider_registry_find(&runtime->providers, provider_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_provider(&runtime->policy, provider->kind, approved);
#endif
    status = UmiAiRuntimeAdmitProvider(runtime, provider_id, approved, &provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return provider->generate(provider->instance, request, response);
}

#endif
/* Legacy dispatch now shares synchronous provider ownership while preserving direct response and provider-status behavior. The former implementation is retained for owner review. */
#if 0
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
/* Generation delegates provider resolution and current policy to the shared admission service. The former inline dispatch checks are preserved for comparison. The former implementation is retained for owner review. */
#if 0
    provider = umi_ai_provider_registry_find(&runtime->providers, provider_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_provider(&runtime->policy, provider->kind, approved);
#endif
    status = UmiAiRequestValidate(request);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiRuntimeAdmitProvider(runtime, provider_id, approved, &provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return provider->generate(provider->instance, request, response);
}

#endif
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
/* Generation delegates provider resolution and current policy to the shared admission service. The former inline dispatch checks are preserved for comparison. The former implementation is retained for owner review. */
#if 0
    provider = umi_ai_provider_registry_find(&runtime->providers, provider_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_policy_check_provider(&runtime->policy, provider->kind, approved);
#endif
    status = UmiAiRequestValidate(request);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiRuntimeAdmitProvider(runtime, provider_id, approved, &provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    int dispatched;
    return UmiAiProviderRegistryInvoke(&runtime->providers, provider, request, response, &dispatched);
}

/*
 * Provide the ai runtime invoke tool operation used by this module and its client
 * applications.
 */
/* Shared tool admission distinguishes corrupt storage from a missing tool before policy and callback execution. The former implementation is retained for owner review. */
#if 0
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

#endif
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
    status = UmiAiToolRegistryResolve(&runtime->tools, tool_id, &tool);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_policy_check_tool(&runtime->policy, approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return tool->invoke(arguments_json, output, output_capacity, tool->user_data);
}

/* Stage the provider's reply separately from the caller's last accepted value.
 * A callback error remains the primary failure even if it left malformed bytes.
 * This is a value publication boundary, not a sandbox for native callbacks. */
/* Checked dispatch uses the same callback lifetime guard and reports a recursive refusal as not dispatched. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiRuntimeGenerateChecked(UmiAiRuntime *runtime,
    const char *provider_id, int approved, const UmiAiRequest *request,
    UmiAiResponse *response, UmiAiGenerationReport *out_report)
{
    UmiAiGenerationReport report = {0};
    UmiAiResponse candidate;
    UmiAiProvider *provider = NULL;
    UmiStatus status;
    report.provider_status = UMI_STATUS_UNAVAILABLE;
    report.response_status = UMI_STATUS_UNAVAILABLE;
    if (runtime == NULL || provider_id == NULL || request == NULL || response == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        status = UmiAiRequestValidate(request);
        if (status == UMI_STATUS_OK)
            status = UmiAiRuntimeAdmitProvider(runtime, provider_id, approved, &provider);
        if (status == UMI_STATUS_OK) {
            umi_ai_response_init(&candidate);
            report.dispatched = 1;
            report.provider_status = provider->generate(provider->instance, request, &candidate);
            status = report.provider_status;
            if (status == UMI_STATUS_OK) {
                report.response_checked = 1;
                report.response_status = UmiAiResponseValidate(&candidate);
                if (report.response_status == UMI_STATUS_OK) {
                    *response = candidate;
                    report.response_published = 1;
                } else {
                    /* A provider returned success with an invalid ABI value. */
                    status = UMI_STATUS_INTERNAL_ERROR;
                }
            }
        }
    }
    report.status = status;
    if (out_report != NULL) *out_report = report;
    return status;
}

#endif
UmiStatus UmiAiRuntimeGenerateChecked(UmiAiRuntime *runtime,
    const char *provider_id, int approved, const UmiAiRequest *request,
    UmiAiResponse *response, UmiAiGenerationReport *out_report)
{
    UmiAiGenerationReport report = {0};
    UmiAiResponse candidate;
    UmiAiProvider *provider = NULL;
    UmiStatus status;
    report.provider_status = UMI_STATUS_UNAVAILABLE;
    report.response_status = UMI_STATUS_UNAVAILABLE;
    if (runtime == NULL || provider_id == NULL || request == NULL || response == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        status = UmiAiRequestValidate(request);
        if (status == UMI_STATUS_OK)
            status = UmiAiRuntimeAdmitProvider(runtime, provider_id, approved, &provider);
        if (status == UMI_STATUS_OK) {
            umi_ai_response_init(&candidate);
            status = UmiAiProviderRegistryInvoke(&runtime->providers, provider,
                request, &candidate, &report.dispatched);
            if (report.dispatched) report.provider_status = status;
            if (status == UMI_STATUS_OK) {
                report.response_checked = 1;
                report.response_status = UmiAiResponseValidate(&candidate);
                if (report.response_status == UMI_STATUS_OK) {
                    *response = candidate;
                    report.response_published = 1;
                } else {
                    /* A provider returned success with an invalid ABI value. */
                    status = UMI_STATUS_INTERNAL_ERROR;
                }
            }
        }
    }
    report.status = status;
    if (out_report != NULL) *out_report = report;
    return status;
}

/* Composition owners preflight callback activity before releasing siblings. */
UmiStatus UmiAiRuntimeCheckIdle(const UmiAiRuntime *runtime)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryCheckIdle(&runtime->providers);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiProviderRegistryValidate(&runtime->providers);
}
