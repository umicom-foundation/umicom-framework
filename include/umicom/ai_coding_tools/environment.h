/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/environment.h
 *
 * PURPOSE:
 *   Bind controlled developer services to AI tools without giving a model direct
 *   access to process handles, filesystem roots, Git implementations or UI state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_ENVIRONMENT_H
#define UMICOM_AI_CODING_TOOLS_ENVIRONMENT_H

#include "umicom/ai_coding_tools/policy.h"
#include "umicom/ai_coding_runtime/workspace.h"
#include "umicom/developer/executor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingCheckpointStore UmiAiCodingCheckpointStore;

typedef struct UmiAiCodingToolEnvironment {
    UmiAiRuntime *ai_runtime;
    UmiAiCodingRuntimePlatform *coding_runtime;
    UmiAiCodingWorkspaceAdapter workspace;
    char workspace_root[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    const UmiDeveloperExecutor *executor;
    UmiDeveloperSourceControl *source_control;
    UmiLanguageService *language;
    UmiDebugRuntimePlatform *debug_runtime;
    UmiAiCodingCheckpointStore *checkpoints;
    UmiAiCodingToolPolicy policy;
    uint64_t revision;
} UmiAiCodingToolEnvironment;

UmiStatus umi_ai_coding_tool_environment_init(
    UmiAiCodingToolEnvironment *environment,
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root);

UmiStatus umi_ai_coding_tool_environment_set_coding_runtime(
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingRuntimePlatform *coding_runtime);

UmiStatus umi_ai_coding_tool_environment_set_executor(
    UmiAiCodingToolEnvironment *environment,
    const UmiDeveloperExecutor *executor);

UmiStatus umi_ai_coding_tool_environment_set_source_control(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperSourceControl *source_control);

UmiStatus umi_ai_coding_tool_environment_set_language(
    UmiAiCodingToolEnvironment *environment,
    UmiLanguageService *language);

UmiStatus umi_ai_coding_tool_environment_set_debug_runtime(
    UmiAiCodingToolEnvironment *environment,
    UmiDebugRuntimePlatform *debug_runtime);

UmiStatus umi_ai_coding_tool_environment_set_checkpoints(
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingCheckpointStore *checkpoints);

UmiStatus umi_ai_coding_tool_environment_set_policy(
    UmiAiCodingToolEnvironment *environment,
    const UmiAiCodingToolPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
