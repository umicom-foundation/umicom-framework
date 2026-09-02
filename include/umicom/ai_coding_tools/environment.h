/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/environment.h
 *
 * PURPOSE:
 *   Bind controlled developer services to AI tools without giving a model direct
 *   access to process handles, filesystem roots, Git implementations or UI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_ENVIRONMENT_H
#define UMICOM_AI_CODING_TOOLS_ENVIRONMENT_H

#include "umicom/ai_coding_tools/policy.h"
#include "umicom/ai_coding_runtime/workspace.h"
#include "umicom/developer/executor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding checkpoint store data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingCheckpointStore UmiAiCodingCheckpointStore;

/**
 * Represent the ai coding tool environment data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise ai coding tool environment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_environment_init(
    UmiAiCodingToolEnvironment *environment,
    UmiAiRuntime *ai_runtime,
    UmiAiCodingRuntimePlatform *coding_runtime,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root);

/**
 * Provide the ai coding tool environment set coding runtime operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_coding_runtime(
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingRuntimePlatform *coding_runtime);

/**
 * Provide the ai coding tool environment set executor operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_executor(
    UmiAiCodingToolEnvironment *environment,
    const UmiDeveloperExecutor *executor);

/**
 * Provide the ai coding tool environment set source control operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_source_control(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperSourceControl *source_control);

/**
 * Provide the ai coding tool environment set language operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_language(
    UmiAiCodingToolEnvironment *environment,
    UmiLanguageService *language);

/**
 * Provide the ai coding tool environment set debug runtime operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_debug_runtime(
    UmiAiCodingToolEnvironment *environment,
    UmiDebugRuntimePlatform *debug_runtime);

/**
 * Provide the ai coding tool environment set checkpoints operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_checkpoints(
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingCheckpointStore *checkpoints);

/**
 * Provide the ai coding tool environment set policy operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_environment_set_policy(
    UmiAiCodingToolEnvironment *environment,
    const UmiAiCodingToolPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
