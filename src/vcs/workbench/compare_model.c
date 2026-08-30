/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/compare_model.c
 *
 * PURPOSE:
 *   Compose existing developer diff computation and advanced VCS session,
 *   summary and navigation records into one operational compare model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/compare_model.h"

#include <string.h>

#include "umicom/vcs/advanced/diff_line.h"

static void convert_options(
    const UmiVcsAdvancedDiffOptions *source,
    UmiDeveloperDiffOptions *destination)
{
    umi_developer_diff_options_init(destination);
    destination->ignore_case = source->ignore_case;
    destination->context_lines = source->context_lines;
    destination->ignore_trailing_whitespace =
        source->whitespace == UMI_VCS_WHITESPACE_IGNORE_TRAILING;
    destination->ignore_all_whitespace =
        source->whitespace == UMI_VCS_WHITESPACE_IGNORE_AMOUNT ||
        source->whitespace == UMI_VCS_WHITESPACE_IGNORE_ALL;
}

static UmiVcsDiffKind row_kind(UmiDeveloperDiffRowKind kind)
{
    switch (kind) {
        case UMI_DEVELOPER_DIFF_INSERT: return UMI_VCS_DIFF_ADDED;
        case UMI_DEVELOPER_DIFF_DELETE: return UMI_VCS_DIFF_DELETED;
        case UMI_DEVELOPER_DIFF_REPLACE: return UMI_VCS_DIFF_MODIFIED;
        case UMI_DEVELOPER_DIFF_EQUAL:
        default: return UMI_VCS_DIFF_CONTEXT;
    }
}

void umi_vcs_workbench_compare_model_init(
    UmiVcsWorkbenchCompareModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    model->view_mode = UMI_VCS_WORKBENCH_SIDE_BY_SIDE;
    model->revision = 1U;
    umi_vcs_advanced_compare_session_init(&model->session);
    umi_vcs_advanced_diff_document_init(&model->document);
    umi_vcs_advanced_diff_options_init(&model->options);
    umi_vcs_advanced_diff_summary_init(&model->summary);
    umi_vcs_advanced_compare_navigation_init(&model->navigation);
}

UmiStatus umi_vcs_workbench_compare_model_open(
    UmiVcsWorkbenchCompareModel *model,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const char *left_text,
    const char *right_text,
    const UmiVcsAdvancedDiffOptions *options)
{
    UmiVcsAdvancedDiffOptions active_options;
    UmiDeveloperDiffOptions developer_options;
    UmiDeveloperDiffDocument *document = NULL;
    UmiDeveloperDiffHunkList hunk_list;
    UmiStatus status;
    size_t index;

    if (model == NULL || session_id == NULL || session_id[0] == '\0' ||
        left_text == NULL || right_text == NULL ||
        umi_vcs_advanced_compare_side_validate(left) != UMI_STATUS_OK ||
        umi_vcs_advanced_compare_side_validate(right) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (options == NULL) {
        umi_vcs_advanced_diff_options_init(&active_options);
    } else {
        if (umi_vcs_advanced_diff_options_validate(options) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        active_options = *options;
    }
    convert_options(&active_options, &developer_options);
    status = umi_developer_diff_document_create(
        left_text, right_text, &developer_options, &document);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&hunk_list, 0, sizeof(hunk_list));
    status = umi_developer_diff_hunks_build(
        document, active_options.context_lines, &hunk_list);
    if (status != UMI_STATUS_OK) {
        umi_developer_diff_document_destroy(document);
        return status;
    }

    umi_vcs_workbench_compare_model_init(model);
    model->options = active_options;
    status = umi_vcs_advanced_compare_session_open(
        &model->session, session_id, left, right, NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_advanced_diff_document_set_paths(
            &model->document, left->path, right->path,
            UMI_VCS_COMPARE_TWO_WAY);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_advanced_copy_text(
            model->document.left_revision,
            sizeof(model->document.left_revision), left->revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_advanced_copy_text(
            model->document.right_revision,
            sizeof(model->document.right_revision), right->revision);
    }
    model->row_count = umi_developer_diff_document_row_count(document);
    if (status == UMI_STATUS_OK &&
        model->row_count > UMI_DEVELOPER_DIFF_MAX_ROWS) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U;
         status == UMI_STATUS_OK && index < model->row_count;
         ++index) {
        UmiVcsAdvancedDiffLine line;
        const char *text;

        status = umi_developer_diff_document_row_at(
            document, index, &model->rows[index]);
        if (status != UMI_STATUS_OK) break;
        text = model->rows[index].kind == UMI_DEVELOPER_DIFF_DELETE
            ? model->rows[index].left_text
            : model->rows[index].right_text;
        umi_vcs_advanced_diff_line_init(&line);
        status = umi_vcs_advanced_diff_line_set(
            &line, row_kind(model->rows[index].kind),
            model->rows[index].left_line,
            model->rows[index].right_line, text);
        if (status == UMI_STATUS_OK) {
            umi_vcs_advanced_diff_summary_add(&model->summary, &line);
        }
    }
    model->hunk_count = hunk_list.count;
    if (status == UMI_STATUS_OK &&
        model->hunk_count > UMI_DEVELOPER_DIFF_MAX_HUNKS) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK && model->hunk_count > 0U) {
        (void)memcpy(model->hunks, hunk_list.hunks,
                     model->hunk_count * sizeof(model->hunks[0]));
    }
    if (status == UMI_STATUS_OK) {
        model->stats = umi_developer_diff_document_stats(document);
        model->document.hunk_count = model->hunk_count;
        model->document.added_lines = model->summary.added_lines;
        model->document.deleted_lines = model->summary.deleted_lines;
        model->document.modified_lines = model->summary.modified_lines;
        model->navigation.change_count = model->hunk_count;
        status = umi_vcs_advanced_compare_session_set_change(
            &model->session, 0U, model->hunk_count);
    }
    if (status == UMI_STATUS_OK) {
        model->current_row = model->hunk_count > 0U
            ? model->hunks[0].first_row : 0U;
        model->ready = 1;
        model->revision += 1U;
    } else {
        umi_vcs_workbench_compare_model_init(model);
    }
    umi_developer_diff_document_destroy(document);
    return status;
}

UmiStatus umi_vcs_workbench_compare_model_set_view_mode(
    UmiVcsWorkbenchCompareModel *model,
    UmiVcsWorkbenchCompareViewMode view_mode)
{
    if (model == NULL || !model->ready ||
        (view_mode != UMI_VCS_WORKBENCH_SIDE_BY_SIDE &&
         view_mode != UMI_VCS_WORKBENCH_INLINE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (model->view_mode != view_mode) {
        model->view_mode = view_mode;
        model->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_compare_model_select_hunk(
    UmiVcsWorkbenchCompareModel *model,
    size_t index)
{
    UmiStatus status;

    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->hunk_count) return UMI_STATUS_NOT_FOUND;
    status = umi_vcs_advanced_compare_session_set_change(
        &model->session, index, model->hunk_count);
    if (status != UMI_STATUS_OK) return status;
    model->navigation.current_index = index;
    model->current_row = model->hunks[index].first_row;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_compare_model_next_hunk(
    UmiVcsWorkbenchCompareModel *model)
{
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_vcs_advanced_compare_navigation_next(&model->navigation)) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_vcs_workbench_compare_model_select_hunk(
        model, model->navigation.current_index);
}

UmiStatus umi_vcs_workbench_compare_model_previous_hunk(
    UmiVcsWorkbenchCompareModel *model)
{
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_vcs_advanced_compare_navigation_previous(&model->navigation)) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_vcs_workbench_compare_model_select_hunk(
        model, model->navigation.current_index);
}

const UmiDeveloperDiffRow *umi_vcs_workbench_compare_model_row_at(
    const UmiVcsWorkbenchCompareModel *model,
    size_t index)
{
    return model != NULL && index < model->row_count
        ? &model->rows[index] : NULL;
}

const UmiDeveloperDiffHunk *umi_vcs_workbench_compare_model_hunk_at(
    const UmiVcsWorkbenchCompareModel *model,
    size_t index)
{
    return model != NULL && index < model->hunk_count
        ? &model->hunks[index] : NULL;
}
