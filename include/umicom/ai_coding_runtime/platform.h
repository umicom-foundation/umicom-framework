/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/platform.h
 *
 * PURPOSE:
 *   Compose the existing AI Runtime, AuthorEngine coding assistant, a real
 *   repository workspace, context indexing, validation and the coding-agent
 *   state machine behind one Framework-owned operational platform.
 *
 * APPLICATION BOUNDARY:
 *   Umicom Studio supplies presentation and user choices only. Future IDEs,
 *   headless automation and teaching tools can compose the same platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PLATFORM_H
#define UMICOM_AI_CODING_RUNTIME_PLATFORM_H

#include "umicom/ai_coding_runtime/agent.h"
#include "umicom/ai_coding_runtime/local_workspace.h"
#include "umicom/ai_coding_runtime/repository_index.h"
#include "umicom/ai_coding_runtime/task_queue.h"
#include "umicom/ai_coding_runtime/validation_selector.h"
#include "umicom/ai_coding_runtime/validation_profiles/profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding runtime platform snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiAiCodingRuntimePlatformSnapshot {
    UmiAiCodingAgentSnapshot agent;
    UmiAiCodingAssistantSnapshot assistant;
    UmiAiCodingRepositoryIndexSnapshot repository;
    size_t queued_events;
    size_t queued_tasks;
    uint64_t revision;
} UmiAiCodingRuntimePlatformSnapshot;

/**
 * Represent the ai coding runtime platform data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingRuntimePlatform UmiAiCodingRuntimePlatform;

/**
 * Provide the ai coding runtime platform create local operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_create_local(
    UmiAiRuntime *ai_runtime,
    UmiAiAuthorEngineService *authorengine,
    const UmiDeveloperExecutor *executor,
    const char *workspace_root,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingRuntimePlatform **out_platform);

/**
 * Release or reset state held by ai coding runtime platform so the same storage can be
 * reused safely.
 */
void umi_ai_coding_runtime_platform_destroy(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform refresh repository operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification);

/**
 * Provide the ai coding runtime platform set validation plan operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan);

/**
 * Provide the ai coding runtime platform detect validation operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection);

/**
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request);

/**
 * Provide the ai coding runtime platform enqueue operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request);

/**
 * Provide the ai coding runtime platform run next operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Return the number of records represented by ai coding runtime platform queued task
 * without changing their state.
 */
size_t umi_ai_coding_runtime_platform_queued_task_count(
    const UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform approve operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by);

/**
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform continue repair operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform reject operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform cancel operation used by this module and its
 * client applications.
 */
void umi_ai_coding_runtime_platform_cancel(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform next event operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_next_event(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingEvent *out_event);

/**
 * Provide the ai coding runtime platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_snapshot(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *out_snapshot);

/**
 * Provide the ai coding runtime platform assistant operation used by this module and its
 * client applications.
 */
UmiAiCodingAssistantService *umi_ai_coding_runtime_platform_assistant(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform agent operation used by this module and its
 * client applications.
 */
UmiAiCodingAgent *umi_ai_coding_runtime_platform_agent(
    UmiAiCodingRuntimePlatform *platform);

/**
 * Provide the ai coding runtime platform workspace root operation used by this module and
 * its client applications.
 */
const char *umi_ai_coding_runtime_platform_workspace_root(
    const UmiAiCodingRuntimePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
