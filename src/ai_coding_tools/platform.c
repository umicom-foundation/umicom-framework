/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/platform.c
 *
 * PURPOSE:
 *   Implement the reusable coding-tools composition root consumed by thin IDEs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/platform.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingToolsPlatform {
    UmiAiCodingToolEnvironment environment;
    UmiAiCodingCheckpointStore checkpoints;
    UmiAiCodingToolExecutor executor;
    UmiAiCodingToolChatRegistry chats;
    UmiAiPolicy previous_ai_policy;
    int ai_policy_saved;
    uint64_t revision;
};

UmiStatus umi_ai_coding_tools_platform_create(
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingToolsPlatform **out_platform)
{
    UmiAiCodingToolsPlatform *platform;
    UmiStatus status;

    if (out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;

    platform = (UmiAiCodingToolsPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->previous_ai_policy = ai_runtime->policy;
    platform->ai_policy_saved = 1;
    ai_runtime->policy.allow_tools = 1;
    ai_runtime->policy.require_tool_approval = 1;

    status = umi_ai_coding_tool_environment_init(
        &platform->environment,
        ai_runtime,
        coding_runtime,
        workspace,
        workspace_root);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_environment_set_executor(
            &platform->environment,
            executor);
    }

    umi_ai_coding_checkpoint_store_init(&platform->checkpoints);

    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_environment_set_checkpoints(
            &platform->environment,
            &platform->checkpoints);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_register_all(&platform->environment);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_executor_init(
            &platform->executor,
            &platform->environment);
    }

    if (status != UMI_STATUS_OK) {
        umi_ai_coding_tools_platform_destroy(platform);
        return status;
    }

    umi_ai_coding_tool_chat_registry_init(&platform->chats);
    platform->revision = 1U;
    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_ai_coding_tools_platform_destroy(
    UmiAiCodingToolsPlatform *platform)
{
    if (platform == NULL) return;

    if (platform->ai_policy_saved &&
        platform->environment.ai_runtime != NULL) {
        platform->environment.ai_runtime->policy =
            platform->previous_ai_policy;
    }

    umi_ai_coding_tool_executor_deinit(&platform->executor);
    free(platform);
}

UmiStatus umi_ai_coding_tools_platform_set_source_control(
    UmiAiCodingToolsPlatform *platform,
    UmiDeveloperSourceControl *source_control)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_environment_set_source_control(
        &platform->environment,
        source_control);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_set_language(
    UmiAiCodingToolsPlatform *platform,
    UmiLanguageService *language)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_environment_set_language(
        &platform->environment,
        language);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_set_debug_runtime(
    UmiAiCodingToolsPlatform *platform,
    UmiDebugRuntimePlatform *debug_runtime)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_environment_set_debug_runtime(
        &platform->environment,
        debug_runtime);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_set_policy(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPolicy *policy)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_environment_set_policy(
        &platform->environment,
        policy);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_open_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const char *provider_id,
    const char *model_id)
{
    UmiAiCodingToolChatSession *session = NULL;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_chat_registry_open(
        &platform->chats,
        session_id,
        provider_id,
        model_id,
        &session);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_run_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const UmiAiCodingToolLoopConfig *config,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result)
{
    UmiAiCodingToolChatSession *session;
    UmiStatus status;

    if (platform == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    session = umi_ai_coding_tool_chat_registry_find(
        &platform->chats,
        session_id);
    if (session == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_ai_coding_tool_agent_loop_run(
        platform->environment.ai_runtime,
        &platform->environment,
        &platform->executor,
        session,
        config,
        user_message,
        out_result);

    platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_tools_platform_execute(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_ai_coding_tool_execute(
        &platform->executor,
        call,
        out_result);
}

UmiStatus umi_ai_coding_tools_platform_execute_plan(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPlan *plan,
    UmiAiCodingToolPlanResult *out_result)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_ai_coding_tool_plan_execute(
        &platform->executor,
        plan,
        out_result);
}

UmiStatus umi_ai_coding_tools_platform_snapshot(
    const UmiAiCodingToolsPlatform *platform,
    UmiAiCodingToolsPlatformSnapshot *out_snapshot)
{
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->tool_count = umi_ai_coding_tool_catalogue_count();
    out_snapshot->result_history_count =
        umi_ai_coding_tool_result_history_count(platform->executor.history);
    out_snapshot->checkpoint_count = platform->checkpoints.count;
    out_snapshot->chat_session_count = platform->chats.count;
    out_snapshot->revision = platform->revision;
    return UMI_STATUS_OK;
}

UmiAiCodingToolEnvironment *umi_ai_coding_tools_platform_environment(
    UmiAiCodingToolsPlatform *platform)
{
    return platform != NULL ? &platform->environment : NULL;
}
