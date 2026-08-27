/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_data_editor.c
 *
 * PURPOSE:
 *   Verify optimistic row-revision validation and edit readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/data/workbench/data_editor.h"

int main(void)
{
    UmiDataEditorModel model;
    UmiDataCellEdit edit = {0};
    assert(umi_data_editor_model_init(&model, "orders", 1) == UMI_STATUS_OK);
    (void)strcpy(edit.edit_id, "edit-1");
    (void)strcpy(edit.row_key, "42");
    (void)strcpy(edit.column, "status");
    (void)strcpy(edit.before_value, "OPEN");
    (void)strcpy(edit.after_value, "FILLED");
    edit.operation = UMI_DATA_EDIT_UPDATE;
    edit.expected_row_revision = 7U;
    assert(umi_data_editor_model_stage(&model, &edit) == UMI_STATUS_OK);
    assert(umi_data_editor_model_validate(&model, "edit-1", 7U) ==
           UMI_STATUS_OK);
    assert(model.ready_to_apply);
    assert(umi_data_editor_model_mark_applied(&model, "edit-1") ==
           UMI_STATUS_OK);
    return 0;
}
