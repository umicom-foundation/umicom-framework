/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_data_editor.c
 *
 * PURPOSE:
 *   Verify optimistic row-revision validation and edit readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "../value_archive/construction_cases.h"
#include <string.h>

#include "umicom/data/workbench/data_editor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted data editor value. */
static int UmiDataEditorModelConstructionCases(void)
{
    UmiDataEditorModel *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_editor_model_init(value, "orders", 1) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->table_name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_editor_model_init(value, oversized, 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_editor_model_init(value, value->table_name, 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->table_name, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiDataEditorModelConstructionCases() != 0) return 1;

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
