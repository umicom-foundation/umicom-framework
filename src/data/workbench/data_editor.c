/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/data_editor.c
 *
 * PURPOSE:
 *   Implement reviewable optimistic row edits without performing provider
 *   mutations outside the canonical Data Server boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/data_editor.h"

#include <string.h>

/* Provide the find edit operation used by this module and its client applications. */
static size_t find_edit(const UmiDataEditorModel *model, const char *edit_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->edit_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->edits[index].edit_id, edit_id) == 0) return index;
    }
    return model->edit_count;
}

/* Provide the refresh operation used by this module and its client applications. */
static void refresh(UmiDataEditorModel *model)
{
    size_t index;
    model->valid_count = 0U;
    model->conflict_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->edit_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->edits[index].state == UMI_DATA_EDIT_VALID) {
            model->valid_count += 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->edits[index].state == UMI_DATA_EDIT_CONFLICTED) {
            model->conflict_count += 1U;
        }
    }
    model->ready_to_apply = model->edit_count > 0U &&
        model->valid_count == model->edit_count && model->conflict_count == 0U;
}

/*
 * Initialise data editor model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_editor_model_init(
    UmiDataEditorModel *model,
    const char *table_name,
    int transaction_required)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || table_name == NULL || table_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_workbench_copy_text(
        model->table_name, sizeof(model->table_name), table_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->transaction_required = transaction_required != 0;
        model->revision = 1U;
    }
    return status;
}

/*
 * Provide the data editor model stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_editor_model_stage(
    UmiDataEditorModel *model,
    const UmiDataCellEdit *edit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || edit == NULL || edit->edit_id[0] == '\0' ||
        edit->row_key[0] == '\0' || edit->column[0] == '\0' ||
        edit->operation < UMI_DATA_EDIT_INSERT ||
        edit->operation > UMI_DATA_EDIT_DELETE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_edit(model, edit->edit_id) < model->edit_count) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->edit_count >= UMI_DATA_WORKBENCH_MAX_EDITS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    model->edits[model->edit_count] = *edit;
    model->edits[model->edit_count].state = UMI_DATA_EDIT_PENDING;
    model->edit_count += 1U;
    refresh(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/* Check that data editor model satisfies its contract before another service relies on it. */
UmiStatus umi_data_editor_model_validate(
    UmiDataEditorModel *model,
    const char *edit_id,
    uint64_t actual_row_revision)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || edit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_edit(model, edit_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->edit_count) return UMI_STATUS_NOT_FOUND;
    model->edits[index].state =
        model->edits[index].expected_row_revision == actual_row_revision
            ? UMI_DATA_EDIT_VALID : UMI_DATA_EDIT_CONFLICTED;
    refresh(model);
    model->revision += 1U;
    return model->edits[index].state == UMI_DATA_EDIT_VALID
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the data editor model mark applied operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_editor_model_mark_applied(
    UmiDataEditorModel *model,
    const char *edit_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || edit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_edit(model, edit_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->edit_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->edits[index].state != UMI_DATA_EDIT_VALID) {
        return UMI_STATUS_INVALID_STATE;
    }
    model->edits[index].state = UMI_DATA_EDIT_APPLIED;
    refresh(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by data editor model so the same storage can be reused
 * safely.
 */
void umi_data_editor_model_clear(UmiDataEditorModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model->edits, 0, sizeof(model->edits));
    model->edit_count = 0U;
    refresh(model);
    model->revision += 1U;
}

/*
 * Find data editor model edit while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDataCellEdit *umi_data_editor_model_edit_at(
    const UmiDataEditorModel *model,
    size_t edit_index)
{
    return model != NULL && edit_index < model->edit_count
        ? &model->edits[edit_index] : NULL;
}
