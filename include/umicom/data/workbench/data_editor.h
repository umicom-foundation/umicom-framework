/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/data_editor.h
 *
 * PURPOSE:
 *   Maintain an optimistic, revision-safe set of reviewed row edits. Applying
 *   provider mutations remains the responsibility of the Data Server adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_DATA_EDITOR_H
#define UMICOM_DATA_WORKBENCH_DATA_EDITOR_H

#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataCellEdit {
    char edit_id[UMI_DATABASE_ID_CAPACITY];
    char row_key[UMI_DATABASE_NAME_CAPACITY];
    char column[UMI_DATABASE_NAME_CAPACITY];
    char before_value[UMI_DATA_WORKBENCH_CELL_CAPACITY];
    char after_value[UMI_DATA_WORKBENCH_CELL_CAPACITY];
    UmiDataEditOperation operation;
    UmiDataEditState state;
    uint64_t expected_row_revision;
} UmiDataCellEdit;

typedef struct UmiDataEditorModel {
    uint32_t struct_size;
    uint32_t api_version;
    char table_name[UMI_DATABASE_NAME_CAPACITY];
    UmiDataCellEdit edits[UMI_DATA_WORKBENCH_MAX_EDITS];
    size_t edit_count;
    size_t valid_count;
    size_t conflict_count;
    int transaction_required;
    int ready_to_apply;
    uint64_t revision;
} UmiDataEditorModel;

UmiStatus umi_data_editor_model_init(
    UmiDataEditorModel *model,
    const char *table_name,
    int transaction_required);
UmiStatus umi_data_editor_model_stage(
    UmiDataEditorModel *model,
    const UmiDataCellEdit *edit);
UmiStatus umi_data_editor_model_validate(
    UmiDataEditorModel *model,
    const char *edit_id,
    uint64_t actual_row_revision);
UmiStatus umi_data_editor_model_mark_applied(
    UmiDataEditorModel *model,
    const char *edit_id);
void umi_data_editor_model_clear(UmiDataEditorModel *model);
const UmiDataCellEdit *umi_data_editor_model_edit_at(
    const UmiDataEditorModel *model,
    size_t edit_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_DATA_EDITOR_H */
