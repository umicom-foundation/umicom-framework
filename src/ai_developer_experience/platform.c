/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/platform.c
 *
 * PURPOSE:
 *   Implement the AI developer experience composition root over existing coding
 *   runtime and coding tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperExperiencePlatform {
    UmiAiCodingRuntimePlatform *coding_runtime;
    UmiAiCodingToolsPlatform *tools;

    UmiAiDeveloperApprovalService approvals;
    UmiAiDeveloperTaskRegistry *tasks;
    UmiAiDeveloperTimeline *timeline;
    UmiAiDeveloperPatchReviewService review;
    UmiAiDeveloperPresentationState presentation;
    UmiAiDeveloperPreferences preferences;
    UmiAiDeveloperActivityCursor activity_cursor;

    UmiAiDeveloperDurableStore *durable;
    char key_prefix[80];
    uint64_t sequence;
    uint64_t revision;
};

/*
 * Provide the append agent events operation used by this module and its client
 * applications.
 */
static UmiStatus append_agent_events(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingEvent event;
    UmiStatus status;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((status = umi_ai_coding_runtime_platform_next_event(
                platform->coding_runtime,
                &event)) == UMI_STATUS_OK) {
        UmiAiDeveloperTimelineEvent timeline_event;

        status = umi_ai_developer_timeline_from_agent_event(
            &event,
            &timeline_event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(
            platform->timeline,
            &timeline_event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return status == UMI_STATUS_NOT_FOUND
        ? UMI_STATUS_OK
        : status;
}

/*
 * Provide the project active task operation used by this module and its client
 * applications.
 */
static UmiStatus project_active_task(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiCodingRuntimePlatformSnapshot *snapshot)
{
    UmiAiDeveloperTaskEntry task;
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char summary[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    UmiStatus status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (snapshot->agent.task_id[0] == '\0') return UMI_STATUS_OK;

    (void)snprintf(
        title,
        sizeof(title),
        "AI Coding Task %.120s",
        snapshot->agent.task_id);
    (void)snprintf(
        summary,
        sizeof(summary),
        "State %s, iteration %u/%u, patch files %zu, validation failures %zu.",
        umi_ai_coding_runtime_state_text(snapshot->agent.state),
        snapshot->agent.iteration,
        snapshot->agent.maximum_iterations,
        snapshot->agent.patch_files,
        snapshot->agent.validation_failures);

    status = umi_ai_developer_task_project(
        &snapshot->agent,
        title,
        summary,
        ++platform->sequence,
        &task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiDeveloperTaskEntry existing;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_developer_task_registry_find(
                platform->tasks,
                task.task_id,
                &existing) == UMI_STATUS_OK) {
            task.created_sequence = existing.created_sequence;
        }
    }

    task.updated_sequence = platform->sequence;

    status = umi_ai_developer_task_registry_upsert(
        platform->tasks,
        &task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this operation only while the related capability or state is available. */
    if (platform->preferences.auto_follow_active_task) {
        (void)umi_ai_developer_presentation_set_task(
            &platform->presentation,
            task.task_id);
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the refresh patch review operation used by this module and its client
 * applications.
 */
static UmiStatus refresh_patch_review(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiCodingRuntimePlatformSnapshot *snapshot)
{
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingPatch patch;
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!snapshot->agent.patch_recorded ||
        snapshot->agent.patch_id[0] == '\0') {
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (platform->review.loaded &&
        strcmp(
            platform->review.review.patch_id,
            snapshot->agent.patch_id) == 0) {
        return UMI_STATUS_OK;
    }

    assistant = umi_ai_coding_runtime_platform_assistant(
        platform->coding_runtime);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistant == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_assistant_find_patch(
        assistant,
        snapshot->agent.patch_id,
        &patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_patch_review_service_load(
        &platform->review,
        &patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_ai_developer_presentation_set_patch(
        &platform->presentation,
        patch.patch_id);

    /* Apply this branch only when its contract condition is satisfied. */
    if (platform->preferences.auto_open_review) {
        (void)umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }

    return UMI_STATUS_OK;
}

/*
 * Initialise ai developer experience platform from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_experience_platform_create(
    UmiAiCodingRuntimePlatform *coding_runtime,
    UmiAiCodingToolsPlatform *tools,
    UmiAiDeveloperExperiencePlatform **out_platform)
{
    UmiAiDeveloperExperiencePlatform *platform;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding_runtime == NULL || tools == NULL ||
        out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_platform = NULL;

    platform = (UmiAiDeveloperExperiencePlatform *)calloc(
        1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->coding_runtime = coding_runtime;
    platform->tools = tools;
    platform->sequence = 1U;
    platform->revision = 1U;

    status = umi_ai_developer_approval_service_init(&platform->approvals);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_task_registry_create(&platform->tasks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_timeline_create(&platform->timeline);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ai_developer_experience_platform_destroy(platform);
        return status;
    }

    umi_ai_developer_patch_review_service_init(&platform->review);
    umi_ai_developer_presentation_state_init(&platform->presentation);
    umi_ai_developer_preferences_init(&platform->preferences);
    umi_ai_developer_activity_cursor_init(&platform->activity_cursor);

    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai developer experience platform so the same storage can
 * be reused safely.
 */
void umi_ai_developer_experience_platform_destroy(
    UmiAiDeveloperExperiencePlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    umi_ai_developer_durable_store_destroy(platform->durable);
    umi_ai_developer_timeline_destroy(platform->timeline);
    umi_ai_developer_task_registry_destroy(platform->tasks);
    umi_ai_developer_approval_service_deinit(&platform->approvals);
    free(platform);
}

/*
 * Provide the ai developer experience platform enable durable state operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_enable_durable_state(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *path,
    const char *key_prefix)
{
    size_t prefix_length;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || path == NULL ||
        key_prefix == NULL || key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    prefix_length = strlen(key_prefix);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (prefix_length >= sizeof(platform->key_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->durable != NULL) {
        umi_ai_developer_durable_store_destroy(platform->durable);
        platform->durable = NULL;
    }

    status = umi_ai_developer_durable_store_create(
        path,
        &platform->durable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memcpy(
        platform->key_prefix,
        key_prefix,
        prefix_length + 1U);
    platform->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer experience platform refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_refresh(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot coding;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_runtime_platform_snapshot(
        platform->coding_runtime,
        &coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = project_active_task(platform, &coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = refresh_patch_review(platform, &coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK &&
        status != UMI_STATUS_NOT_FOUND) {
        return status;
    }

    status = append_agent_events(platform);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiCodingToolResultHistory *history =
            umi_ai_coding_tools_platform_history(platform->tools);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (history != NULL) {
            status = umi_ai_developer_activity_sync_tools(
                &platform->activity_cursor,
                platform->presentation.active_task_id,
                history,
                platform->timeline);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    {
        UmiAiCodingAgent *agent =
            umi_ai_coding_runtime_platform_agent(platform->coding_runtime);
        const UmiAiCodingValidationReport *validation =
            umi_ai_coding_agent_last_validation(agent);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (validation != NULL && validation->result_count > 0U) {
            status = umi_ai_developer_activity_sync_validation(
                &platform->activity_cursor,
                platform->presentation.active_task_id,
                validation,
                platform->timeline);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Write ai developer experience platform in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_ai_developer_experience_platform_save(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiSessionStore *store;
    UmiAiCodingToolChatRegistry *chats;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->durable == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    store = umi_ai_developer_durable_store_session(platform->durable);
    chats = umi_ai_coding_tools_platform_chats(platform->tools);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || chats == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_developer_session_bundle_save(
        store,
        platform->key_prefix,
        chats,
        platform->tasks,
        platform->approvals.queue,
        &platform->presentation,
        &platform->preferences);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_durable_store_save(platform->durable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform restore operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_restore(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperRestoreReport *out_report)
{
    UmiSessionStore *store;
    UmiAiCodingToolChatRegistry *chats;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->durable == NULL ||
        out_report == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_durable_store_load(platform->durable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    store = umi_ai_developer_durable_store_session(platform->durable);
    chats = umi_ai_coding_tools_platform_chats(platform->tools);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || chats == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_developer_session_bundle_restore(
        store,
        platform->key_prefix,
        chats,
        platform->tasks,
        platform->approvals.queue,
        &platform->presentation,
        &platform->preferences,
        out_report);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform select pane operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_pane(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_presentation_set_pane(
        &platform->presentation, pane);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform select approval operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_approval(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_presentation_set_approval(
        &platform->presentation,
        approval_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_APPROVALS);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform approve operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_approve(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id,
    const char *approved_by)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || approval_id == NULL ||
        approved_by == NULL || approved_by[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request.state != UMI_AI_DEVELOPER_APPROVAL_PENDING) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_approval_approve(
        &platform->approvals,
        approval_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request.kind == UMI_AI_DEVELOPER_APPROVAL_PATCH) {
        status = umi_ai_coding_runtime_platform_approve(
            platform->coding_runtime,
            approved_by);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (request.executable &&
               request.tool_call.tool_id[0] != '\0') {
        UmiAiCodingToolResult result;
        UmiAiCodingToolCall call = request.tool_call;

        call.approved = 1;

        status = umi_ai_coding_tools_platform_execute(
            platform->tools,
            &call,
            &result);

        (void)umi_ai_developer_approval_queue_set_state(
            platform->approvals.queue,
            approval_id,
            status == UMI_STATUS_OK
                ? UMI_AI_DEVELOPER_APPROVAL_EXECUTED
                : UMI_AI_DEVELOPER_APPROVAL_FAILED);

        {
            UmiAiDeveloperTimelineEvent event;
            (void)umi_ai_developer_timeline_from_tool_result(
                platform->presentation.active_task_id,
                &result,
                &event);
            (void)umi_ai_developer_timeline_append(
                platform->timeline,
                &event);
        }
    }

    {
        UmiAiDeveloperApprovalRequest decided;
        UmiAiDeveloperTimelineEvent event;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_developer_approval_queue_find(
                platform->approvals.queue,
                approval_id,
                &decided) == UMI_STATUS_OK &&
            umi_ai_developer_timeline_from_approval(
                &decided,
                &event) == UMI_STATUS_OK) {
            (void)umi_ai_developer_timeline_append(
                platform->timeline,
                &event);
        }
    }

    platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform reject operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_reject(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request.state != UMI_AI_DEVELOPER_APPROVAL_PENDING) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_approval_reject(
        &platform->approvals,
        approval_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request.kind == UMI_AI_DEVELOPER_APPROVAL_PATCH) {
        status = umi_ai_coding_runtime_platform_reject(
            platform->coding_runtime);
    }

    {
        UmiAiDeveloperApprovalRequest decided;
        UmiAiDeveloperTimelineEvent event;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_developer_approval_queue_find(
                platform->approvals.queue,
                approval_id,
                &decided) == UMI_STATUS_OK &&
            umi_ai_developer_timeline_from_approval(
                &decided,
                &event) == UMI_STATUS_OK) {
            (void)umi_ai_developer_timeline_append(
                platform->timeline,
                &event);
        }
    }

    platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform open patch operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_open_patch(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *patch_id)
{
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingPatch patch;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || patch_id == NULL ||
        patch_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    assistant = umi_ai_coding_runtime_platform_assistant(
        platform->coding_runtime);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistant == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_assistant_find_patch(
        assistant, patch_id, &patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_patch_review_service_load(
        &platform->review, &patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_patch(
            &platform->presentation, patch_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform select patch file operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_patch_file(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_patch_review_service_select_file(
        &platform->review,
        file_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_diff(
            &platform->presentation,
            platform->review.diff.path,
            0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform mark patch file reviewed operation used by
 * this module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_mark_patch_file_reviewed(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index,
    int reviewed)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_patch_review_service_mark_reviewed(
        &platform->review,
        file_index,
        reviewed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ai developer experience platform snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_snapshot(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperExperienceSnapshot *out_snapshot)
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

    status = umi_ai_coding_runtime_platform_snapshot(
        platform->coding_runtime,
        &out_snapshot->coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tools_platform_snapshot(
            platform->tools,
            &out_snapshot->tools);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->presentation = platform->presentation;
    out_snapshot->preferences = platform->preferences;
    out_snapshot->task_count =
        umi_ai_developer_task_registry_count(platform->tasks);
    out_snapshot->pending_approval_count =
        umi_ai_developer_approval_queue_pending_count(
            platform->approvals.queue);
    out_snapshot->timeline_count =
        umi_ai_developer_timeline_count(platform->timeline);
    out_snapshot->revision = platform->revision;
    out_snapshot->review_loaded = platform->review.loaded;
    out_snapshot->durable_state_enabled = platform->durable != NULL;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer experience platform approvals operation used by this module and
 * its client applications.
 */
UmiAiDeveloperApprovalService *
umi_ai_developer_experience_platform_approvals(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->approvals : NULL;
}

/*
 * Provide the ai developer experience platform tasks operation used by this module and its
 * client applications.
 */
UmiAiDeveloperTaskRegistry *
umi_ai_developer_experience_platform_tasks(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->tasks : NULL;
}

/*
 * Provide the ai developer experience platform timeline operation used by this module and
 * its client applications.
 */
UmiAiDeveloperTimeline *
umi_ai_developer_experience_platform_timeline(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->timeline : NULL;
}

/*
 * Provide the ai developer experience platform review operation used by this module and
 * its client applications.
 */
UmiAiDeveloperPatchReviewService *
umi_ai_developer_experience_platform_review(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->review : NULL;
}

/*
 * Provide the ai developer experience platform presentation operation used by this module
 * and its client applications.
 */
UmiAiDeveloperPresentationState *
umi_ai_developer_experience_platform_presentation(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->presentation : NULL;
}

/*
 * Provide the ai developer experience platform preferences operation used by this module
 * and its client applications.
 */
UmiAiDeveloperPreferences *
umi_ai_developer_experience_platform_preferences(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->preferences : NULL;
}

/*
 * Provide the ai developer experience platform coding operation used by this module and
 * its client applications.
 */
UmiAiCodingRuntimePlatform *
umi_ai_developer_experience_platform_coding(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->coding_runtime : NULL;
}

/*
 * Provide the ai developer experience platform tools operation used by this module and its
 * client applications.
 */
UmiAiCodingToolsPlatform *
umi_ai_developer_experience_platform_tools(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->tools : NULL;
}
