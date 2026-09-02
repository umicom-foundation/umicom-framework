/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/platform.h
 *
 * PURPOSE:
 *   Compose tool registration, policy, approval-aware execution, checkpoints,
 *   multi-step plans and repository-aware chat behind one Framework boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLATFORM_H
#define UMICOM_AI_CODING_TOOLS_PLATFORM_H

#include "umicom/ai_coding_tools/agent_loop.h"
#include "umicom/ai_coding_tools/chat_registry.h"
#include "umicom/ai_coding_tools/checkpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding tools platform snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolsPlatformSnapshot {
    size_t tool_count;
    size_t result_history_count;
    size_t checkpoint_count;
    size_t chat_session_count;
    uint64_t revision;
} UmiAiCodingToolsPlatformSnapshot;

/**
 * Represent the ai coding tools platform data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolsPlatform UmiAiCodingToolsPlatform;

/**
 * Initialise ai coding tools platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tools_platform_create(
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingToolsPlatform **out_platform);

/**
 * Release or reset state held by ai coding tools platform so the same storage can be
 * reused safely.
 */
void umi_ai_coding_tools_platform_destroy(
    UmiAiCodingToolsPlatform *platform);

/**
 * Provide the ai coding tools platform set source control operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tools_platform_set_source_control(
    UmiAiCodingToolsPlatform *platform,
    UmiDeveloperSourceControl *source_control);

/**
 * Provide the ai coding tools platform set language operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_set_language(
    UmiAiCodingToolsPlatform *platform,
    UmiLanguageService *language);

/**
 * Provide the ai coding tools platform set debug runtime operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tools_platform_set_debug_runtime(
    UmiAiCodingToolsPlatform *platform,
    UmiDebugRuntimePlatform *debug_runtime);

/**
 * Provide the ai coding tools platform set policy operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_set_policy(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPolicy *policy);

/**
 * Provide the ai coding tools platform open chat operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_open_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const char *provider_id,
    const char *model_id);

/**
 * Provide the ai coding tools platform run chat operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_run_chat(
    UmiAiCodingToolsPlatform *platform,
    const char *session_id,
    const UmiAiCodingToolLoopConfig *config,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result);

/**
 * Perform ai coding tools platform through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ai_coding_tools_platform_execute(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result);

/**
 * Provide the ai coding tools platform execute plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_execute_plan(
    UmiAiCodingToolsPlatform *platform,
    const UmiAiCodingToolPlan *plan,
    UmiAiCodingToolPlanResult *out_result);

/**
 * Provide the ai coding tools platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tools_platform_snapshot(
    const UmiAiCodingToolsPlatform *platform,
    UmiAiCodingToolsPlatformSnapshot *out_snapshot);

/**
 * Provide the ai coding tools platform environment operation used by this module and its
 * client applications.
 */
UmiAiCodingToolEnvironment *umi_ai_coding_tools_platform_environment(
    UmiAiCodingToolsPlatform *platform);

/**
 * Provide the ai coding tools platform chats operation used by this module and its client
 * applications.
 */
UmiAiCodingToolChatRegistry *umi_ai_coding_tools_platform_chats(
    UmiAiCodingToolsPlatform *platform);

/**
 * Provide the ai coding tools platform history operation used by this module and its
 * client applications.
 */
UmiAiCodingToolResultHistory *umi_ai_coding_tools_platform_history(
    UmiAiCodingToolsPlatform *platform);

/**
 * Provide the ai coding tools platform checkpoints operation used by this module and its
 * client applications.
 */
UmiAiCodingCheckpointStore *umi_ai_coding_tools_platform_checkpoints(
    UmiAiCodingToolsPlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
