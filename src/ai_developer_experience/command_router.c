/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/command_router.c
 *
 * PURPOSE:
 *   Implement command routing for pane selection, approval decisions, patch
 *   review navigation and durable session operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/command_router.h"

#include <stdio.h>
#include <string.h>

static UmiAiDeveloperPaneKind pane_for_command(const char *command_id)
{
    const UmiAiDeveloperCommandDescriptor *descriptor =
        umi_ai_developer_command_find(command_id);

    return descriptor != NULL
        ? descriptor->target_pane
        : UMI_AI_DEVELOPER_PANE_OVERVIEW;
}

int umi_ai_developer_command_enabled(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *command_id,
    const UmiAiDeveloperCommandContext *context)
{
    const UmiAiDeveloperCommandDescriptor *descriptor;
    UmiAiDeveloperPatchReviewService *review;
    UmiAiDeveloperPresentationState *presentation;
    UmiAiDeveloperApprovalRequest approval;

    if (platform == NULL || command_id == NULL || context == NULL) {
        return 0;
    }

    descriptor = umi_ai_developer_command_find(command_id);
    if (descriptor == NULL) return 0;

    review = umi_ai_developer_experience_platform_review(platform);
    presentation =
        umi_ai_developer_experience_platform_presentation(platform);

    if (descriptor->requires_review &&
        (review == NULL || !review->loaded)) {
        return 0;
    }

    if (descriptor->requires_active_approval) {
        const char *approval_id =
            context->approval_id[0] != '\0'
                ? context->approval_id
                : (presentation != NULL
                    ? presentation->active_approval_id
                    : "");

        if (approval_id[0] == '\0' ||
            umi_ai_developer_approval_queue_find(
                umi_ai_developer_experience_platform_approvals(platform)->queue,
                approval_id,
                &approval) != UMI_STATUS_OK ||
            approval.state != UMI_AI_DEVELOPER_APPROVAL_PENDING) {
            return 0;
        }
    }

    if (strcmp(command_id, "ai.developer.new-chat") == 0) {
        return context->chat_session_id[0] != '\0' &&
            context->provider_id[0] != '\0' &&
            context->model_id[0] != '\0';
    }

    if (strcmp(command_id, "ai.developer.patch-apply") == 0) {
        return review != NULL &&
            umi_ai_developer_patch_review_service_ready_to_approve(review);
    }

    return 1;
}

static UmiStatus open_pane(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *command_id)
{
    return umi_ai_developer_experience_platform_select_pane(
        platform,
        pane_for_command(command_id));
}

static UmiStatus new_chat(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiDeveloperCommandContext *context)
{
    UmiAiCodingToolsPlatform *tools;
    UmiAiDeveloperPresentationState *presentation;
    UmiStatus status;

    tools = umi_ai_developer_experience_platform_tools(platform);
    presentation =
        umi_ai_developer_experience_platform_presentation(platform);

    if (tools == NULL || presentation == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tools_platform_open_chat(
        tools,
        context->chat_session_id,
        context->provider_id,
        context->model_id);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_presentation_set_chat(
        presentation,
        context->chat_session_id);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_experience_platform_select_pane(
            platform,
            UMI_AI_DEVELOPER_PANE_CHAT);
    }

    return status;
}

static UmiStatus approve(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiDeveloperCommandContext *context)
{
    UmiAiDeveloperPresentationState *presentation =
        umi_ai_developer_experience_platform_presentation(platform);
    const char *approval_id =
        context->approval_id[0] != '\0'
            ? context->approval_id
            : presentation->active_approval_id;

    return umi_ai_developer_experience_platform_approve(
        platform,
        approval_id,
        context->approved_by);
}

static UmiStatus reject(
    UmiAiDeveloperExperiencePlatform *platform,
    const UmiAiDeveloperCommandContext *context)
{
    UmiAiDeveloperPresentationState *presentation =
        umi_ai_developer_experience_platform_presentation(platform);
    const char *approval_id =
        context->approval_id[0] != '\0'
            ? context->approval_id
            : presentation->active_approval_id;

    return umi_ai_developer_experience_platform_reject(
        platform,
        approval_id);
}

static UmiStatus diff_move(
    UmiAiDeveloperExperiencePlatform *platform,
    int forward)
{
    UmiAiDeveloperPatchReviewService *review =
        umi_ai_developer_experience_platform_review(platform);
    UmiAiDeveloperPresentationState *presentation =
        umi_ai_developer_experience_platform_presentation(platform);
    size_t next = 0U;
    UmiStatus status;

    if (review == NULL || presentation == NULL || !review->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = forward
        ? umi_ai_developer_diff_next_change(
            &review->diff,
            presentation->active_diff_line,
            &next)
        : umi_ai_developer_diff_previous_change(
            &review->diff,
            presentation->active_diff_line,
            &next);

    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_presentation_set_diff(
        presentation,
        review->diff.path,
        next);
}

UmiStatus umi_ai_developer_command_execute(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *command_id,
    const UmiAiDeveloperCommandContext *context,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status = UMI_STATUS_NOT_IMPLEMENTED;

    if (platform == NULL || command_id == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_ai_developer_command_enabled(
            platform, command_id, context)) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (strncmp(command_id, "ai.developer.open-", 18U) == 0) {
        status = open_pane(platform, command_id);
    } else if (strcmp(command_id, "ai.developer.new-chat") == 0) {
        status = new_chat(platform, context);
    } else if (strcmp(
                   command_id,
                   "ai.developer.approval-approve") == 0) {
        status = approve(platform, context);
    } else if (strcmp(
                   command_id,
                   "ai.developer.approval-reject") == 0) {
        status = reject(platform, context);
    } else if (strcmp(command_id, "ai.developer.patch-select-file") == 0) {
        status = umi_ai_developer_experience_platform_select_patch_file(
            platform,
            context->file_index);
    } else if (strcmp(
                   command_id,
                   "ai.developer.patch-mark-reviewed") == 0) {
        status =
            umi_ai_developer_experience_platform_mark_patch_file_reviewed(
                platform,
                context->file_index,
                1);
    } else if (strcmp(
                   command_id,
                   "ai.developer.patch-mark-unreviewed") == 0) {
        status =
            umi_ai_developer_experience_platform_mark_patch_file_reviewed(
                platform,
                context->file_index,
                0);
    } else if (strcmp(command_id, "ai.developer.patch-apply") == 0) {
        status = umi_ai_coding_runtime_platform_apply(
            umi_ai_developer_experience_platform_coding(platform));
    } else if (strcmp(command_id, "ai.developer.patch-reject") == 0) {
        status = umi_ai_coding_runtime_platform_reject(
            umi_ai_developer_experience_platform_coding(platform));
    } else if (strcmp(command_id, "ai.developer.diff-next") == 0) {
        status = diff_move(platform, 1);
    } else if (strcmp(command_id, "ai.developer.diff-previous") == 0) {
        status = diff_move(platform, 0);
    } else if (strcmp(command_id, "ai.developer.session-save") == 0) {
        status = umi_ai_developer_experience_platform_save(platform);
    } else if (strcmp(command_id, "ai.developer.session-restore") == 0) {
        UmiAiDeveloperRestoreReport report;
        status = umi_ai_developer_experience_platform_restore(
            platform,
            &report);
    }

    if (out_message != NULL && message_capacity > 0U) {
        const UmiAiDeveloperCommandDescriptor *descriptor =
            umi_ai_developer_command_find(command_id);

        (void)snprintf(
            out_message,
            message_capacity,
            "%s: %s",
            descriptor != NULL ? descriptor->label : command_id,
            status == UMI_STATUS_OK ? "completed" : "failed");
    }

    return status;
}
