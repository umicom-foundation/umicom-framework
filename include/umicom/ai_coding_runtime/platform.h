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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiCodingRuntimePlatformSnapshot {
    UmiAiCodingAgentSnapshot agent;
    UmiAiCodingAssistantSnapshot assistant;
    UmiAiCodingRepositoryIndexSnapshot repository;
    size_t queued_events;
    size_t queued_tasks;
    uint64_t revision;
} UmiAiCodingRuntimePlatformSnapshot;

typedef struct UmiAiCodingRuntimePlatform UmiAiCodingRuntimePlatform;

UmiStatus umi_ai_coding_runtime_platform_create_local(
    UmiAiRuntime *ai_runtime,
    UmiAiAuthorEngineService *authorengine,
    const UmiDeveloperExecutor *executor,
    const char *workspace_root,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingRuntimePlatform **out_platform);

void umi_ai_coding_runtime_platform_destroy(
    UmiAiCodingRuntimePlatform *platform);

UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification);

UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan);

UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection);

UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request);

UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request);

UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform);

size_t umi_ai_coding_runtime_platform_queued_task_count(
    const UmiAiCodingRuntimePlatform *platform);

UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by);

UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform);

UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform);

UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform);

void umi_ai_coding_runtime_platform_cancel(
    UmiAiCodingRuntimePlatform *platform);

UmiStatus umi_ai_coding_runtime_platform_next_event(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingEvent *out_event);

UmiStatus umi_ai_coding_runtime_platform_snapshot(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *out_snapshot);

UmiAiCodingAssistantService *umi_ai_coding_runtime_platform_assistant(
    UmiAiCodingRuntimePlatform *platform);

UmiAiCodingAgent *umi_ai_coding_runtime_platform_agent(
    UmiAiCodingRuntimePlatform *platform);

const char *umi_ai_coding_runtime_platform_workspace_root(
    const UmiAiCodingRuntimePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
