/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/data_editor.c
 *
 * PURPOSE:
 *   Implement reviewable optimistic row edits without performing provider
 *   mutations outside the canonical Data Server boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/data_editor.h"

#include <string.h>

static size_t find_edit(const UmiDataEditorModel *model, const char *edit_id)
{
    size_t index;
    for (index = 0U; index < model->edit_count; ++index) {
        if (strcmp(model->edits[index].edit_id, edit_id) == 0) return index;
    }
    return model->edit_count;
}

static void refresh(UmiDataEditorModel *model)
{
    size_t index;
    model->valid_count = 0U;
    model->conflict_count = 0U;
    for (index = 0U; index < model->edit_count; ++index) {
        if (model->edits[index].state == UMI_DATA_EDIT_VALID) {
            model->valid_count += 1U;
        } else if (model->edits[index].state == UMI_DATA_EDIT_CONFLICTED) {
            model->conflict_count += 1U;
        }
    }
    model->ready_to_apply = model->edit_count > 0U &&
        model->valid_count == model->edit_count && model->conflict_count == 0U;
}

UmiStatus umi_data_editor_model_init(
    UmiDataEditorModel *model,
    const char *table_name,
    int transaction_required)
{
    UmiStatus status;
    if (model == NULL || table_name == NULL || table_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_workbench_copy_text(
        model->table_name, sizeof(model->table_name), table_name);
    if (status == UMI_STATUS_OK) {
        model->transaction_required = transaction_required != 0;
        model->revision = 1U;
    }
    return status;
}

UmiStatus umi_data_editor_model_stage(
    UmiDataEditorModel *model,
    const UmiDataCellEdit *edit)
{
    if (model == NULL || edit == NULL || edit->edit_id[0] == '\0' ||
        edit->row_key[0] == '\0' || edit->column[0] == '\0' ||
        edit->operation < UMI_DATA_EDIT_INSERT ||
        edit->operation > UMI_DATA_EDIT_DELETE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_edit(model, edit->edit_id) < model->edit_count) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
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

UmiStatus umi_data_editor_model_validate(
    UmiDataEditorModel *model,
    const char *edit_id,
    uint64_t actual_row_revision)
{
    size_t index;
    if (model == NULL || edit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_edit(model, edit_id);
    if (index >= model->edit_count) return UMI_STATUS_NOT_FOUND;
    model->edits[index].state =
        model->edits[index].expected_row_revision == actual_row_revision
            ? UMI_DATA_EDIT_VALID : UMI_DATA_EDIT_CONFLICTED;
    refresh(model);
    model->revision += 1U;
    return model->edits[index].state == UMI_DATA_EDIT_VALID
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_data_editor_model_mark_applied(
    UmiDataEditorModel *model,
    const char *edit_id)
{
    size_t index;
    if (model == NULL || edit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_edit(model, edit_id);
    if (index >= model->edit_count) return UMI_STATUS_NOT_FOUND;
    if (model->edits[index].state != UMI_DATA_EDIT_VALID) {
        return UMI_STATUS_INVALID_STATE;
    }
    model->edits[index].state = UMI_DATA_EDIT_APPLIED;
    refresh(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_data_editor_model_clear(UmiDataEditorModel *model)
{
    if (model == NULL) return;
    (void)memset(model->edits, 0, sizeof(model->edits));
    model->edit_count = 0U;
    refresh(model);
    model->revision += 1U;
}

const UmiDataCellEdit *umi_data_editor_model_edit_at(
    const UmiDataEditorModel *model,
    size_t edit_index)
{
    return model != NULL && edit_index < model->edit_count
        ? &model->edits[edit_index] : NULL;
}
