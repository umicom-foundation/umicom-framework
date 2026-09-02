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

/*
 * Provide the ai coding runtime platform create local operation used by this module and
 * its client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ai_runtime == NULL || authorengine == NULL ||
        workspace_root == NULL || workspace_root[0] == '\0' ||
        config == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_runtime_config_validate(config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    root_length = strlen(workspace_root);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (root_length >= UMI_AI_CODING_RUNTIME_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_platform = NULL;

    platform = (UmiAiCodingRuntimePlatform *)calloc(1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_local_workspace_adapter(
            platform->local_workspace,
            &platform->workspace);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_assistant_create(
            authorengine,
            &platform->assistant);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_repository_index_create(
            umi_ai_coding_assistant_context(platform->assistant),
            &platform->repository_index);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_task_queue_create(
            &platform->task_queue);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ai_coding_runtime_platform_destroy(platform);
        return status;
    }

    platform->revision = 1U;
    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding runtime platform so the same storage can be
 * reused safely.
 */
void umi_ai_coding_runtime_platform_destroy(
    UmiAiCodingRuntimePlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    umi_ai_coding_task_queue_destroy(platform->task_queue);
    umi_ai_coding_agent_destroy(platform->agent);
    umi_ai_coding_repository_index_destroy(platform->repository_index);
    umi_ai_coding_assistant_destroy(platform->assistant);
    umi_ai_coding_local_workspace_destroy(platform->local_workspace);
    free(platform);
}

/*
 * Provide the ai coding runtime platform refresh repository operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_repository_index_refresh(
        platform->repository_index,
        platform->workspace_root,
        &platform->ignore_policy,
        classification);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform set validation plan operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_set_validation_plan(
        platform->agent,
        plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform detect validation operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection)
{
    UmiAiCodingValidationPlan plan;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_validation_select(
        &platform->workspace,
        platform->workspace_root,
        out_selection,
        &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_agent_set_validation_plan(
        platform->agent,
        &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_run(
        platform->agent,
        task_id,
        request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform enqueue operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiAiCodingQueuedTask task;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || task_id == NULL ||
        task_id[0] == '\0' || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&task, 0, sizeof(task));

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform run next operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingAgentSnapshot state;
    UmiAiCodingQueuedTask task;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_snapshot(
        platform->agent,
        &state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (state.state != UMI_AI_CODING_RUNTIME_IDLE &&
        state.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        state.state != UMI_AI_CODING_RUNTIME_FAILED &&
        state.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    status = umi_ai_coding_task_queue_pop(
        platform->task_queue,
        &task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_agent_run(
        platform->agent,
        task.task_id,
        &task.request);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Return the number of records represented by ai coding runtime platform queued task
 * without changing their state.
 */
size_t umi_ai_coding_runtime_platform_queued_task_count(
    const UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL
        ? umi_ai_coding_task_queue_count(platform->task_queue)
        : 0U;
}

/*
 * Provide the ai coding runtime platform approve operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_approve_pending(
        platform->agent,
        approved_by);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_apply_pending(platform->agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform continue repair operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_continue_repair(platform->agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform reject operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_agent_reject_pending(platform->agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai coding runtime platform cancel operation used by this module and its
 * client applications.
 */
void umi_ai_coding_runtime_platform_cancel(
    UmiAiCodingRuntimePlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform != NULL) {
        umi_ai_coding_agent_cancel(platform->agent);
        platform->revision += 1U;
    }
}

/*
 * Provide the ai coding runtime platform next event operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_next_event(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_agent_next_event(platform->agent, out_event);
}

/*
 * Provide the ai coding runtime platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_runtime_platform_snapshot(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_ai_coding_agent_snapshot(
        platform->agent,
        &out_snapshot->agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_assistant_snapshot(
        platform->assistant,
        &out_snapshot->assistant);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_repository_index_snapshot(
        platform->repository_index,
        &out_snapshot->repository);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the ai coding runtime platform assistant operation used by this module and its
 * client applications.
 */
UmiAiCodingAssistantService *umi_ai_coding_runtime_platform_assistant(
    UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->assistant : NULL;
}

/*
 * Provide the ai coding runtime platform agent operation used by this module and its
 * client applications.
 */
UmiAiCodingAgent *umi_ai_coding_runtime_platform_agent(
    UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->agent : NULL;
}

/*
 * Provide the ai coding runtime platform workspace root operation used by this module and
 * its client applications.
 */
const char *umi_ai_coding_runtime_platform_workspace_root(
    const UmiAiCodingRuntimePlatform *platform)
{
    return platform != NULL ? platform->workspace_root : NULL;
}
