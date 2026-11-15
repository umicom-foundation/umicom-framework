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
    int operation_active;
    int cancellation_observed;
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

/* A composition owner must retain every borrowed child until its outer
 * operation returns, including callbacks entered through child accessors. */
UmiStatus UmiAiCodingRuntimePlatformCheckIdle(const UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return UMI_STATUS_OK;
    if (platform->operation_active) return UMI_STATUS_BUSY;
    UmiStatus status = UmiAiCodingAgentCheckIdle(platform->agent);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiCodingAssistantCheckIdle(platform->assistant);
}

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
/* Propagate busy child ownership before releasing workspace, queue or repository dependencies; preserve checked handles on refusal. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus UmiAiCodingRuntimePlatformDestroyChecked(UmiAiCodingRuntimePlatform *platform)
{
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK || platform == NULL) return status;
    platform->operation_active = 1;
    status = UmiAiCodingAgentDestroyChecked(platform->agent);
    if (status != UMI_STATUS_OK) { platform->operation_active = 0; return status; }
    platform->agent = NULL;
    umi_ai_coding_task_queue_destroy(platform->task_queue); platform->task_queue = NULL;
    umi_ai_coding_repository_index_destroy(platform->repository_index); platform->repository_index = NULL;
    status = UmiAiCodingAssistantDestroyChecked(platform->assistant);
    if (status != UMI_STATUS_OK) { platform->operation_active = 0; return status; }
    platform->assistant = NULL;
    umi_ai_coding_local_workspace_destroy(platform->local_workspace);
    free(platform); return UMI_STATUS_OK;
}

void umi_ai_coding_runtime_platform_destroy(UmiAiCodingRuntimePlatform *platform)
{
    (void)UmiAiCodingRuntimePlatformDestroyChecked(platform);
}

/* Compare semantic fields, never structure padding or unused bytes after a
 * string terminator. Extend this comparison when snapshot contracts add fields. */
static int coding_platform_observations_equal(const UmiAiCodingRuntimePlatformSnapshot *left,
    const UmiAiCodingRuntimePlatformSnapshot *right)
{
    return left->agent.state == right->agent.state &&
        left->agent.last_status == right->agent.last_status &&
        left->agent.iteration == right->agent.iteration &&
        left->agent.maximum_iterations == right->agent.maximum_iterations &&
        left->agent.context_files == right->agent.context_files &&
        left->agent.patch_files == right->agent.patch_files &&
        left->agent.validation_steps == right->agent.validation_steps &&
        left->agent.validation_failures == right->agent.validation_failures &&
        left->agent.revision == right->agent.revision &&
        left->agent.cancelled == right->agent.cancelled &&
        left->agent.patch_recorded == right->agent.patch_recorded &&
        left->agent.patch_applied == right->agent.patch_applied &&
        left->agent.validation_passed == right->agent.validation_passed &&
        left->agent.rolled_back == right->agent.rolled_back &&
        left->assistant.revision == right->assistant.revision &&
        left->assistant.repository_files == right->assistant.repository_files &&
        left->assistant.task_count == right->assistant.task_count &&
        left->assistant.patch_count == right->assistant.patch_count &&
        left->assistant.draft_patches == right->assistant.draft_patches &&
        left->assistant.approved_patches == right->assistant.approved_patches &&
        left->assistant.applied_patches == right->assistant.applied_patches &&
        left->assistant.reverted_patches == right->assistant.reverted_patches &&
        left->assistant.patch_policy.maximum_files == right->assistant.patch_policy.maximum_files &&
        left->assistant.patch_policy.maximum_changed_lines == right->assistant.patch_policy.maximum_changed_lines &&
        left->assistant.patch_policy.allow_create == right->assistant.patch_policy.allow_create &&
        left->assistant.patch_policy.allow_delete == right->assistant.patch_policy.allow_delete &&
        left->assistant.patch_policy.require_approval == right->assistant.patch_policy.require_approval &&
        left->repository.scanned_files == right->repository.scanned_files &&
        left->repository.indexed_files == right->repository.indexed_files &&
        left->repository.ignored_non_text_files == right->repository.ignored_non_text_files &&
        left->repository.capacity_dropped_files == right->repository.capacity_dropped_files &&
        left->repository.revision == right->repository.revision &&
        left->repository.truncated == right->repository.truncated &&
        left->queued_events == right->queued_events &&
        left->queued_tasks == right->queued_tasks &&
        strncmp(left->agent.task_id, right->agent.task_id, sizeof(left->agent.task_id)) == 0 &&
        strncmp(left->agent.request_id, right->agent.request_id, sizeof(left->agent.request_id)) == 0 &&
        strncmp(left->agent.provider_id, right->agent.provider_id, sizeof(left->agent.provider_id)) == 0 &&
        strncmp(left->agent.model_id, right->agent.model_id, sizeof(left->agent.model_id)) == 0 &&
        strncmp(left->agent.patch_id, right->agent.patch_id, sizeof(left->agent.patch_id)) == 0 &&
        strncmp(left->assistant.last_request_id, right->assistant.last_request_id, sizeof(left->assistant.last_request_id)) == 0 &&
        strncmp(left->assistant.last_patch_id, right->assistant.last_patch_id, sizeof(left->assistant.last_patch_id)) == 0;
}


/* Admission reserves the only parent revision this serialized operation can
 * consume. Cancellation during callbacks is deferred into that same revision. */
static UmiStatus coding_platform_begin_observation(UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *before)
{
    if (platform == NULL || before == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    if (platform->revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (platform->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_ai_coding_runtime_platform_snapshot(platform, before);
    if (status != UMI_STATUS_OK) return status;
    platform->cancellation_observed = 0;
    platform->operation_active = 1;
    return UMI_STATUS_OK;
}


/* Child operations retain their original status. Success already invalidates
 * the parent observation. After failure, inspect children to detect partial
 * work; if capture itself fails, conservatively invalidate without replacing
 * the primary operation error with the secondary observation error. */
static UmiStatus coding_platform_finish_observation(UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingRuntimePlatformSnapshot *before, UmiStatus operation_status)
{
    int changed = operation_status == UMI_STATUS_OK || platform->cancellation_observed;
    if (!changed) {
        UmiAiCodingRuntimePlatformSnapshot after;
        UmiStatus observed = umi_ai_coding_runtime_platform_snapshot(platform, &after);
        changed = observed != UMI_STATUS_OK ||
            !coding_platform_observations_equal(before, &after);
    }
    if (changed) ++platform->revision;
    platform->cancellation_observed = 0;
    platform->operation_active = 0;
    return operation_status;
}

/*
 * Provide the ai coding runtime platform refresh repository operation used by this module
 * and its client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_refresh_repository_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_refresh_repository_internal(platform, classification);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_refresh_repository(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiDataClassification classification)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_refresh_repository_internal(platform, classification);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform set validation plan operation used by this module
 * and its client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_set_validation_plan_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_set_validation_plan_internal(platform, plan);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_set_validation_plan(
    UmiAiCodingRuntimePlatform *platform,
    const UmiAiCodingValidationPlan *plan)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_set_validation_plan_internal(platform, plan);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform detect validation operation used by this module
 * and its client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_detect_validation_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_detect_validation_internal(platform, out_selection);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_detect_validation(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingValidationSelection *out_selection)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_detect_validation_internal(platform, out_selection);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_run_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_run_internal(platform, task_id, request);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_run(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_run_internal(platform, task_id, request);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform enqueue operation used by this module and its
 * client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Reuse bounded draft construction for immediate and queued coding tasks. It replaces unbounded identity scans and preserves queue state on invalid input; the former implementation is retained for review. The former implementation is retained for owner review. */
#if 0
static UmiStatus coding_platform_enqueue_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

#endif
static UmiStatus coding_platform_enqueue_internal(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiCodingQueuedTask task;
    UmiStatus status = UmiAiCodingQueuedTaskPrepare(task_id, request, &task);
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_coding_task_queue_push(platform->task_queue, &task);
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_enqueue_internal(platform, task_id, request);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_enqueue(
    UmiAiCodingRuntimePlatform *platform,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_enqueue_internal(platform, task_id, request);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform run next operation used by this module and its
 * client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_run_next_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_run_next_internal(platform);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_run_next(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_run_next_internal(platform);
    return coding_platform_finish_observation(platform, &before, status);
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
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_approve_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_approve_internal(platform, approved_by);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_approve(
    UmiAiCodingRuntimePlatform *platform,
    const char *approved_by)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_approve_internal(platform, approved_by);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Perform ai coding runtime platform through the module contract so client applications do
 * not duplicate its policy.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_apply_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_apply_internal(platform);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_apply(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_apply_internal(platform);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform continue repair operation used by this module and
 * its client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_continue_repair_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_continue_repair_internal(platform);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_continue_repair(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_continue_repair_internal(platform);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform reject operation used by this module and its
 * client applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus coding_platform_reject_internal(
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
/* Parent revision publication now belongs to the guarded outer operation, which also observes failed child changes and deferred cancellation. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK) platform->revision += 1U;
#endif

    return status;
}

/* Reserve revision headroom before child work and publish one parent observation for successful or observed failed changes while preserving the primary result. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingRuntimePlatformCheckIdle(platform);
    if (status != UMI_STATUS_OK) return status;
    platform->operation_active = 1;
    status = coding_platform_reject_internal(platform);
    platform->operation_active = 0;
    return status;
}

#endif
UmiStatus umi_ai_coding_runtime_platform_reject(
    UmiAiCodingRuntimePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot before;
    UmiStatus status = coding_platform_begin_observation(platform, &before);
    if (status != UMI_STATUS_OK) return status;
    status = coding_platform_reject_internal(platform);
    return coding_platform_finish_observation(platform, &before, status);
}

/*
 * Provide the ai coding runtime platform cancel operation used by this module and its
 * client applications.
 */
/* Cancellation remains available at counter exhaustion. Active cancellation shares the outer observation revision; idle cancellation saturates rather than wrapping. The former implementation is retained for owner review. */
#if 0
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

#endif

void umi_ai_coding_runtime_platform_cancel(UmiAiCodingRuntimePlatform *platform)
{
    if (platform == NULL) return;
    /* Stopping work must remain possible when observation metadata is exhausted.
     * During active callbacks the outer operation already reserved its revision;
     * record cancellation there instead of consuming its slot or wrapping. */
    umi_ai_coding_agent_cancel(platform->agent);
    if (platform->operation_active) platform->cancellation_observed = 1;
    else if (platform->revision != 0U && platform->revision != UINT64_MAX) ++platform->revision;
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
/* Publish a complete composition snapshot only after every child observation succeeds, preserving the previous UI state on refusal. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_ai_coding_runtime_platform_snapshot(
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingRuntimePlatformSnapshot *out_snapshot)
{
    UmiStatus status;
    UmiAiCodingRuntimePlatformSnapshot candidate = {0};

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* The bounded observation contains values only. Staging it locally avoids
     * exposing a partial child capture and does not allocate or acquire a lock. */

    status = umi_ai_coding_agent_snapshot(
        platform->agent,
        &candidate.agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_assistant_snapshot(
        platform->assistant,
        &candidate.assistant);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_repository_index_snapshot(
        platform->repository_index,
        &candidate.repository);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Events are intentionally drained only by the consumer. The snapshot keeps
     * a simple revision rather than peeking into the opaque queue internals.
     */
    candidate.queued_events =
        umi_ai_coding_agent_event_count(platform->agent);
    candidate.queued_tasks =
        umi_ai_coding_task_queue_count(platform->task_queue);
    candidate.revision = platform->revision;
    *out_snapshot = candidate;
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


/* Keep hosting applications on the composition boundary while the agent
 * retains ownership of the latest completed patch call observation. */
UmiStatus UmiAiCodingRuntimePlatformReadPatchOutcome(
    const UmiAiCodingRuntimePlatform *platform, UmiAiCodingAgentPatchOutcome *outOutcome)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiCodingAgentReadPatchOutcome(platform->agent, outOutcome);
}
