/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/platform.h
 *
 * PURPOSE:
 *   Compose tool registration, policy, approval-aware execution, checkpoints,
 *   multi-step plans and repository-aware chat behind one Framework boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLATFORM_H
#define UMICOM_AI_CODING_TOOLS_PLATFORM_H

#include "umicom/ai_coding_tools/agent_loop.h"
#include "umicom/ai_coding_tools/chat_registry.h"
#include "umicom/ai_coding_tools/checkpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingToolsPlatformSnapshot {
    size_t tool_count;
    size_t result_history_count;
    size_t checkpoint_count;
    size_t chat_session_count;
    uint64_t revision;
} UmiAiCodingToolsPlatformSnapshot;

typedef struct UmiAiCodingToolsPlatform UmiAiCodingToolsPlatform;

UmiStatus umi_ai_coding_tools_platform_create(
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingToolsPlatform **out_platform);

void umi_ai_coding_tools_platform_destroy(
    UmiAiCodingToolsPlatform *platform);

UmiStatus umi_ai_coding_tools_platform_set_source_control(
    UmiAiCodingToolsPlatform *platform,
    UmiDeveloperSourceControl *source_control);

UmiStatus umi_ai_coding_tools_platform_set_language(
    UmiAiCodingToolsPlatform *platform,
    UmiLanguageService *language);

UmiStatus umi_ai_coding_tools_platform_set_debug_runtime(
    UmiAiCodingToolsPlatform *platform,
    UmiDebugRuntimePlatform *debug_runtime);

UmiStatus umi_ai_coding_tools_platform_set_policy(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPolicy *policy);

UmiStatus umi_ai_coding_tools_platform_open_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const char *provider_id,
    const char *model_id);

UmiStatus umi_ai_coding_tools_platform_run_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const UmiAiCodingToolLoopConfig *config,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result);

UmiStatus umi_ai_coding_tools_platform_execute(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result);

UmiStatus umi_ai_coding_tools_platform_execute_plan(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPlan *plan,
    UmiAiCodingToolPlanResult *out_result);

UmiStatus umi_ai_coding_tools_platform_snapshot(
    const UmiAiCodingToolsPlatform *platform,
    UmiAiCodingToolsPlatformSnapshot *out_snapshot);

UmiAiCodingToolEnvironment *umi_ai_coding_tools_platform_environment(
    UmiAiCodingToolsPlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
