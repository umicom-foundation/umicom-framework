/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/diff.c
 *
 * PURPOSE:
 *   Render unified or split line diff evidence into a toolkit-neutral view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/diff.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

/* Provide the unified rows operation used by this module and its client applications. */
static UmiStatus unified_rows(
    UmiUiViewModel *view,
    const UmiAiDeveloperFileDiff *diff,
    size_t first_row,
    size_t visible_rows)
{
    size_t end;
    size_t index;
    size_t row = 0U;
    UmiStatus status = UMI_STATUS_OK;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first_row >= diff->line_count) first_row = 0U;

    end = first_row + visible_rows;
    /* Apply this branch only when its contract condition is satisfied. */
    if (end > diff->line_count) end = diff->line_count;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = first_row; index < end; ++index) {
        const UmiAiDeveloperDiffLine *line = &diff->lines[index];
        char key[96];
        char text[UMI_UI_VALUE_STRING_CAPACITY];
        const char marker =
            line->kind == UMI_AI_DEVELOPER_DIFF_ADD ? '+' :
            line->kind == UMI_AI_DEVELOPER_DIFF_REMOVE ? '-' : ' ';

        (void)snprintf(key, sizeof(key), "ai-diff.row.%zu", row++);
        (void)snprintf(
            text,
            sizeof(text),
            "%c %5u %5u | %.450s",
            marker,
            line->old_line,
            line->new_line,
            line->text);

        status = umi_ai_developer_view_set_string(view, key, text);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_view_set_integer(
            view, "ai-diff.row-count", (int64_t)row);
    }

    return status;
}

/* Provide the split rows operation used by this module and its client applications. */
static UmiStatus split_rows(
    UmiUiViewModel *view,
    const UmiAiDeveloperSideBySideDiff *split,
    size_t first_row,
    size_t visible_rows)
{
    size_t end;
    size_t index;
    size_t row = 0U;
    UmiStatus status = UMI_STATUS_OK;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first_row >= split->row_count) first_row = 0U;

    end = first_row + visible_rows;
    /* Apply this branch only when its contract condition is satisfied. */
    if (end > split->row_count) end = split->row_count;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = first_row; index < end; ++index) {
        const UmiAiDeveloperSideBySideRow *item = &split->rows[index];
        char key[96];
        char text[UMI_UI_VALUE_STRING_CAPACITY];

        (void)snprintf(key, sizeof(key), "ai-diff.row.%zu", row++);
        (void)snprintf(
            text,
            sizeof(text),
            "%5u | %.205s || %5u | %.205s",
            item->left_line,
            item->left_text,
            item->right_line,
            item->right_text);

        status = umi_ai_developer_view_set_string(view, key, text);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_view_set_integer(
            view, "ai-diff.row-count", (int64_t)row);
    }

    return status;
}

/*
 * Initialise ai developer diff view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_diff_view_create(
    const char *view_id,
    const UmiAiDeveloperPatchReviewService *review,
    const UmiAiDeveloperPreferences *preferences,
    size_t first_row,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || preferences == NULL ||
        !review->loaded) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_preferences_validate(preferences);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-diff",
        "AI Diff Review",
        "Line-oriented governed patch review with unified and side-by-side layouts.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_view_set_string(
        *out_view, "ai-diff.path", review->diff.path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view,
            "ai-diff.layout",
            preferences->diff_layout ==
                    UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE
                ? "side-by-side"
                : "unified");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-diff.added",
            (int64_t)review->diff.added_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-diff.removed",
            (int64_t)review->diff.removed_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-diff.hunk-count",
            (int64_t)review->diff.hunk_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-diff.truncated",
            review->diff.truncated || review->split.truncated);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status =
            preferences->diff_layout ==
                    UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE
                ? split_rows(
                    *out_view,
                    &review->split,
                    first_row,
                    preferences->visible_rows)
                : unified_rows(
                    *out_view,
                    &review->diff,
                    first_row,
                    preferences->visible_rows);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_diff_previous(),
            "Previous Change",
            "Jump to the previous changed line",
            review->diff.added_count + review->diff.removed_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_diff_next(),
            "Next Change",
            "Jump to the next changed line",
            review->diff.added_count + review->diff.removed_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_review(),
            "Patch Review",
            "Return to patch file review",
            1);

    return status;
}
