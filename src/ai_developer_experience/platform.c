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

static UmiStatus append_agent_events(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingEvent event;
    UmiStatus status;

    while ((status = umi_ai_coding_runtime_platform_next_event(
                platform->coding_runtime,
                &event)) == UMI_STATUS_OK) {
        UmiAiDeveloperTimelineEvent timeline_event;

        status = umi_ai_developer_timeline_from_agent_event(
            &event,
            &timeline_event);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(
            platform->timeline,
            &timeline_event);
        if (status != UMI_STATUS_OK) return status;
    }

    return status == UMI_STATUS_NOT_FOUND
        ? UMI_STATUS_OK
        : status;
}

static UmiStatus project_active_task(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiCodingRuntimePlatformSnapshot *snapshot)
{
    UmiAiDeveloperTaskEntry task;
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char summary[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    UmiStatus status;

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
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiDeveloperTaskEntry existing;

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
    if (status != UMI_STATUS_OK) return status;

    if (platform->preferences.auto_follow_active_task) {
        (void)umi_ai_developer_presentation_set_task(
            &platform->presentation,
            task.task_id);
    }

    return UMI_STATUS_OK;
}

static UmiStatus refresh_patch_review(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiCodingRuntimePlatformSnapshot *snapshot)
{
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingPatch patch;
    UmiStatus status;

    if (!snapshot->agent.patch_recorded ||
        snapshot->agent.patch_id[0] == '\0') {
        return UMI_STATUS_OK;
    }

    if (platform->review.loaded &&
        strcmp(
            platform->review.review.patch_id,
            snapshot->agent.patch_id) == 0) {
        return UMI_STATUS_OK;
    }

    assistant = umi_ai_coding_runtime_platform_assistant(
        platform->coding_runtime);
    if (assistant == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_assistant_find_patch(
        assistant,
        snapshot->agent.patch_id,
        &patch);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_patch_review_service_load(
        &platform->review,
        &patch);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_ai_developer_presentation_set_patch(
        &platform->presentation,
        patch.patch_id);

    if (platform->preferences.auto_open_review) {
        (void)umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_experience_platform_create(
    UmiAiCodingRuntimePlatform *coding_runtime,
    UmiAiCodingToolsPlatform *tools,
    UmiAiDeveloperExperiencePlatform **out_platform)
{
    UmiAiDeveloperExperiencePlatform *platform;
    UmiStatus status;

    if (coding_runtime == NULL || tools == NULL ||
        out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_platform = NULL;

    platform = (UmiAiDeveloperExperiencePlatform *)calloc(
        1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->coding_runtime = coding_runtime;
    platform->tools = tools;
    platform->sequence = 1U;
    platform->revision = 1U;

    status = umi_ai_developer_approval_service_init(&platform->approvals);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_task_registry_create(&platform->tasks);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_timeline_create(&platform->timeline);
    }
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

void umi_ai_developer_experience_platform_destroy(
    UmiAiDeveloperExperiencePlatform *platform)
{
    if (platform == NULL) return;

    umi_ai_developer_durable_store_destroy(platform->durable);
    umi_ai_developer_timeline_destroy(platform->timeline);
    umi_ai_developer_task_registry_destroy(platform->tasks);
    umi_ai_developer_approval_service_deinit(&platform->approvals);
    free(platform);
}

UmiStatus umi_ai_developer_experience_platform_enable_durable_state(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *path,
    const char *key_prefix)
{
    size_t prefix_length;
    UmiStatus status;

    if (platform == NULL || path == NULL ||
        key_prefix == NULL || key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    prefix_length = strlen(key_prefix);
    if (prefix_length >= sizeof(platform->key_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (platform->durable != NULL) {
        umi_ai_developer_durable_store_destroy(platform->durable);
        platform->durable = NULL;
    }

    status = umi_ai_developer_durable_store_create(
        path,
        &platform->durable);
    if (status != UMI_STATUS_OK) return status;

    (void)memcpy(
        platform->key_prefix,
        key_prefix,
        prefix_length + 1U);
    platform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_experience_platform_refresh(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatformSnapshot coding;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_runtime_platform_snapshot(
        platform->coding_runtime,
        &coding);
    if (status != UMI_STATUS_OK) return status;

    status = project_active_task(platform, &coding);
    if (status != UMI_STATUS_OK) return status;

    status = refresh_patch_review(platform, &coding);
    if (status != UMI_STATUS_OK &&
        status != UMI_STATUS_NOT_FOUND) {
        return status;
    }

    status = append_agent_events(platform);
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiCodingToolResultHistory *history =
            umi_ai_coding_tools_platform_history(platform->tools);

        if (history != NULL) {
            status = umi_ai_developer_activity_sync_tools(
                &platform->activity_cursor,
                platform->presentation.active_task_id,
                history,
                platform->timeline);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    {
        UmiAiCodingAgent *agent =
            umi_ai_coding_runtime_platform_agent(platform->coding_runtime);
        const UmiAiCodingValidationReport *validation =
            umi_ai_coding_agent_last_validation(agent);

        if (validation != NULL && validation->result_count > 0U) {
            status = umi_ai_developer_activity_sync_validation(
                &platform->activity_cursor,
                platform->presentation.active_task_id,
                validation,
                platform->timeline);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_experience_platform_save(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiSessionStore *store;
    UmiAiCodingToolChatRegistry *chats;
    UmiStatus status;

    if (platform == NULL || platform->durable == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    store = umi_ai_developer_durable_store_session(platform->durable);
    chats = umi_ai_coding_tools_platform_chats(platform->tools);

    if (store == NULL || chats == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_developer_session_bundle_save(
        store,
        platform->key_prefix,
        chats,
        platform->tasks,
        platform->approvals.queue,
        &platform->presentation,
        &platform->preferences);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_durable_store_save(platform->durable);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_restore(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperRestoreReport *out_report)
{
    UmiSessionStore *store;
    UmiAiCodingToolChatRegistry *chats;
    UmiStatus status;

    if (platform == NULL || platform->durable == NULL ||
        out_report == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_durable_store_load(platform->durable);
    if (status != UMI_STATUS_OK) return status;

    store = umi_ai_developer_durable_store_session(platform->durable);
    chats = umi_ai_coding_tools_platform_chats(platform->tools);

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

    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_select_pane(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_presentation_set_pane(
        &platform->presentation, pane);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_select_approval(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    if (platform == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_presentation_set_approval(
        &platform->presentation,
        approval_id);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_APPROVALS);
    }
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_approve(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id,
    const char *approved_by)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    if (platform == NULL || approval_id == NULL ||
        approved_by == NULL || approved_by[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    if (status != UMI_STATUS_OK) return status;

    if (request.state != UMI_AI_DEVELOPER_APPROVAL_PENDING) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_approval_approve(
        &platform->approvals,
        approval_id);
    if (status != UMI_STATUS_OK) return status;

    if (request.kind == UMI_AI_DEVELOPER_APPROVAL_PATCH) {
        status = umi_ai_coding_runtime_platform_approve(
            platform->coding_runtime,
            approved_by);
    } else if (request.executable &&
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

UmiStatus umi_ai_developer_experience_platform_reject(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id)
{
    UmiAiDeveloperApprovalRequest request;
    UmiStatus status;

    if (platform == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_approval_queue_find(
        platform->approvals.queue,
        approval_id,
        &request);
    if (status != UMI_STATUS_OK) return status;

    if (request.state != UMI_AI_DEVELOPER_APPROVAL_PENDING) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_approval_reject(
        &platform->approvals,
        approval_id);
    if (status != UMI_STATUS_OK) return status;

    if (request.kind == UMI_AI_DEVELOPER_APPROVAL_PATCH) {
        status = umi_ai_coding_runtime_platform_reject(
            platform->coding_runtime);
    }

    {
        UmiAiDeveloperApprovalRequest decided;
        UmiAiDeveloperTimelineEvent event;

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

UmiStatus umi_ai_developer_experience_platform_open_patch(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *patch_id)
{
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingPatch patch;
    UmiStatus status;

    if (platform == NULL || patch_id == NULL ||
        patch_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    assistant = umi_ai_coding_runtime_platform_assistant(
        platform->coding_runtime);
    if (assistant == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_assistant_find_patch(
        assistant, patch_id, &patch);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_patch_review_service_load(
        &platform->review, &patch);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_patch(
            &platform->presentation, patch_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_pane(
            &platform->presentation,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_select_patch_file(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_patch_review_service_select_file(
        &platform->review,
        file_index);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_set_diff(
            &platform->presentation,
            platform->review.diff.path,
            0U);
    }
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_mark_patch_file_reviewed(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index,
    int reviewed)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_patch_review_service_mark_reviewed(
        &platform->review,
        file_index,
        reviewed);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_experience_platform_snapshot(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperExperienceSnapshot *out_snapshot)
{
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_ai_coding_runtime_platform_snapshot(
        platform->coding_runtime,
        &out_snapshot->coding);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tools_platform_snapshot(
            platform->tools,
            &out_snapshot->tools);
    }
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

UmiAiDeveloperApprovalService *
umi_ai_developer_experience_platform_approvals(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->approvals : NULL;
}

UmiAiDeveloperTaskRegistry *
umi_ai_developer_experience_platform_tasks(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->tasks : NULL;
}

UmiAiDeveloperTimeline *
umi_ai_developer_experience_platform_timeline(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->timeline : NULL;
}

UmiAiDeveloperPatchReviewService *
umi_ai_developer_experience_platform_review(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->review : NULL;
}

UmiAiDeveloperPresentationState *
umi_ai_developer_experience_platform_presentation(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->presentation : NULL;
}

UmiAiDeveloperPreferences *
umi_ai_developer_experience_platform_preferences(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? &platform->preferences : NULL;
}

UmiAiCodingRuntimePlatform *
umi_ai_developer_experience_platform_coding(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->coding_runtime : NULL;
}

UmiAiCodingToolsPlatform *
umi_ai_developer_experience_platform_tools(
    UmiAiDeveloperExperiencePlatform *platform)
{
    return platform != NULL ? platform->tools : NULL;
}
