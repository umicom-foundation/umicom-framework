/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/platform.c
 *
 * PURPOSE:
 *   Implement the complete local repository-aware AI coding composition root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/platform.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingRuntimePlatform {
    UmiAiRuntime *ai_runtime;
    UmiAiAuthorEngineService *authorengine;
    const UmiDeveloperExecutor *executor;

    UmiAiCodingLocalWorkspace *local_workspace;
    UmiAiCodingWorkspaceAdapter workspace;
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingRepositoryIndex *repository_index;
    UmiAiCodingAgent *agent;
    UmiAiCodingTaskQueue *task_queue;
    UmiAiCodingIgnorePolicy ignore_policy;

    char workspace_root[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    uint64_t revision;
};

UmiStatus umi_ai_coding_runtime_platform_create_local(
    UmiAiRuntime *ai_runtime,
    UmiAiAuthorEngineService *authorengine,
    const UmiDeveloperExecutor *executor,
    const char *workspace_root,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingRuntimePlatform **out_platform)
{
    UmiAiCodingRuntimePlatform *platform;
    UmiAiCodingAgentBindings bindings;
    UmiStatus status;
    size_t root_length;

    if (ai_runtime == NULL || authorengine == NULL ||
        workspace_root == NULL || workspace_root[0] == '\0' ||
        config == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_runtime_config_validate(config);
    if (status != UMI_STATUS_OK) return status;

    root_length = strlen(workspace_root);
    if (root_length >= UMI_AI_CODING_RUNTIME_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_platform = NULL;

    platform = (UmiAiCodingRuntimePlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->ai_runtime = ai_runtime;
    platform->authorengine = authorengine;
    platform->executor = executor;
    (void)memcpy(
        platform->workspace_root,
        workspace_root,
        root_length + 1U);
    umi_ai_coding_ignore_policy_init(&platform->ignore_policy);

    status = umi_ai_coding_local_workspace_create(
        workspace_root,
        &platform->local_workspace);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_local_workspace_adapter(
            platform->local_workspace,
            &platform->workspace);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_assistant_create(
            authorengine,
            &platform->assistant);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_repository_index_create(
            umi_ai_coding_assistant_context(platform->assistant),
            &platform->repository_index);
    }

    if (status == UMI_STATUS_OK) {
        (void)memset(&bindings, 0, sizeof(bindings));
        bindings.ai_runtime = ai_runtime;
        bindings.assistant = platform->assistant;
        bindings.workspace = platform->workspace;
        bindings.executor = executor;

        status = umi_ai_coding_agent_create(
            &bindings,
            config,
            &platform->agent);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_task_queue_create(
            &platform->task_queue);
    }

    if (status != UMI_STATUS_OK) {
        umi_ai_coding_runtime_platform_destroy(platform);
        return status;
    }

    platform->revision = 1U;
    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_ai_coding_runtime_platform_destroy(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return;

    umi_ai_coding_task_queue_destroy(platform->task_queue);
    umi_ai_coding_agent_destroy(platform->agent);
    umi_ai_coding_repository_index_destroy(platform->repository_index);
    umi_ai_coding_assistant_destroy(platform->assistant);
    umi_ai_coding_local_workspace_destroy(platform->local_workspace);
    free(platform);
}

UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_repository_index_refresh(
        platform->repository_index,
        platform->workspace_root,
        &platform->ignore_policy,
        classification);

    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_set_validation_plan(
        platform->agent,
        plan);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection)
{
    UmiAiCodingValidationPlan plan;
    UmiStatus status;

    if (platform == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_validation_select(
        &platform->workspace,
        platform->workspace_root,
        out_selection,
        &plan);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_agent_set_validation_plan(
        platform->agent,
        &plan);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_run(
        platform->agent,
        task_id,
        request);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiAiCodingQueuedTask task;
    UmiStatus status;

    if (platform == NULL || task_id == NULL ||
        task_id[0] == '\0' || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&task, 0, sizeof(task));

    if (strlen(task_id) >= sizeof(task.task_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(
        task.task_id,
        task_id,
        strlen(task_id) + 1U);
    task.request = *request;

    status = umi_ai_coding_task_queue_push(
        platform->task_queue,
        &task);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingAgentSnapshot state;
    UmiAiCodingQueuedTask task;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_snapshot(
        platform->agent,
        &state);
    if (status != UMI_STATUS_OK) return status;

    if (state.state != UMI_AI_CODING_RUNTIME_IDLE &&
        state.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        state.state != UMI_AI_CODING_RUNTIME_FAILED &&
        state.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    status = umi_ai_coding_task_queue_pop(
        platform->task_queue,
        &task);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_agent_run(
        platform->agent,
        task.task_id,
        &task.request);

    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

size_t umi_ai_coding_runtime_platform_queued_task_count(
    const UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL
        ? umi_ai_coding_task_queue_count(platform->task_queue)
        : 0U;
}

UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_approve_pending(
        platform->agent,
        approved_by);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_apply_pending(platform->agent);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_continue_repair(platform->agent);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_reject_pending(platform->agent);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

void umi_ai_coding_runtime_platform_cancel(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform != NULL) {
        umi_ai_coding_agent_cancel(platform->agent);
        platform->revision += 1U;
    }
}

UmiStatus umi_ai_coding_runtime_platform_next_event(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingEvent *out_event)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_agent_next_event(platform->agent, out_event);
}

UmiStatus umi_ai_coding_runtime_platform_snapshot(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_ai_coding_agent_snapshot(
        platform->agent,
        &out_snapshot->agent);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_assistant_snapshot(
        platform->assistant,
        &out_snapshot->assistant);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_repository_index_snapshot(
        platform->repository_index,
        &out_snapshot->repository);
    if (status != UMI_STATUS_OK) return status;

    /*
     * Events are intentionally drained only by the consumer. The snapshot keeps
     * a simple revision rather than peeking into the opaque queue internals.
     */
    out_snapshot->queued_events =
        umi_ai_coding_agent_event_count(platform->agent);
    out_snapshot->queued_tasks =
        umi_ai_coding_task_queue_count(platform->task_queue);
    out_snapshot->revision = platform->revision;
    return UMI_STATUS_OK;
}

UmiAiCodingAssistantService *umi_ai_coding_runtime_platform_assistant(
    UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->assistant : NULL;
}

UmiAiCodingAgent *umi_ai_coding_runtime_platform_agent(
    UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->agent : NULL;
}

const char *umi_ai_coding_runtime_platform_workspace_root(
    const UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->workspace_root : NULL;
}
