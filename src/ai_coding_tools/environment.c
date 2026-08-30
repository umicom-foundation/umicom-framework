/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/environment.c
 *
 * PURPOSE:
 *   Implement tool service binding and policy replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/environment.h"

#include <string.h>

UmiStatus umi_ai_coding_tool_environment_init(
    UmiAiCodingToolEnvironment *environment,
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root)
{
    size_t length;
    UmiStatus status;

    if (environment == NULL || ai_runtime == NULL ||
        workspace == NULL ||
        workspace_root == NULL || workspace_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    if (status != UMI_STATUS_OK) return status;

    length = strlen(workspace_root);
    if (length >= sizeof(environment->workspace_root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(environment, 0, sizeof(*environment));
    environment->ai_runtime = ai_runtime;
    environment->coding_runtime = coding_runtime;
    environment->workspace = *workspace;
    (void)memcpy(environment->workspace_root, workspace_root, length + 1U);
    umi_ai_coding_tool_policy_init(&environment->policy);
    environment->revision = 1U;
    return UMI_STATUS_OK;
}

#define SETTER(name, field, type) \
UmiStatus name(UmiAiCodingToolEnvironment *environment, type value) \
{ \
    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    environment->field = value; \
    environment->revision += 1U; \
    return UMI_STATUS_OK; \
}

SETTER(
    umi_ai_coding_tool_environment_set_coding_runtime,
    coding_runtime,
    UmiAiCodingRuntimePlatform *)
SETTER(
    umi_ai_coding_tool_environment_set_executor,
    executor,
    const UmiDeveloperExecutor *)
SETTER(
    umi_ai_coding_tool_environment_set_source_control,
    source_control,
    UmiDeveloperSourceControl *)
SETTER(
    umi_ai_coding_tool_environment_set_language,
    language,
    UmiLanguageService *)
SETTER(
    umi_ai_coding_tool_environment_set_debug_runtime,
    debug_runtime,
    UmiDebugRuntimePlatform *)
SETTER(
    umi_ai_coding_tool_environment_set_checkpoints,
    checkpoints,
    UmiAiCodingCheckpointStore *)

#undef SETTER

UmiStatus umi_ai_coding_tool_environment_set_policy(
    UmiAiCodingToolEnvironment *environment,
    const UmiAiCodingToolPolicy *policy)
{
    if (environment == NULL || policy == NULL ||
        policy->maximum_plan_steps == 0U ||
        policy->maximum_plan_steps > UMI_AI_CODING_TOOL_PLAN_CAPACITY ||
        policy->maximum_output_bytes == 0U ||
        policy->maximum_output_bytes > UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    environment->policy = *policy;
    environment->revision += 1U;
    return UMI_STATUS_OK;
}
