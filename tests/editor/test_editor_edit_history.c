/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_edit_history.c
 *
 * PURPOSE:
 *   Implement the test editor edit history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework edit history tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/editor/edit_history.h"

int main(void)
{
    UmiEditorEditHistory *history = NULL;
    UmiEditorEditRecord edit = {0};
    UmiEditorEditRecord operation;

    edit.operation_id = 7U;
    edit.offset = 3U;
    edit.removed_byte_count = 3U;
    edit.inserted_byte_count = 4U;
    (void)memcpy(edit.removed_bytes, "old", 3U);
    (void)memcpy(edit.inserted_bytes, "new!", 4U);
    edit.before_revision = 10U;
    edit.after_revision = 11U;
    if (umi_editor_edit_history_create(&history) != UMI_STATUS_OK) return 1;
    if (umi_editor_edit_history_push(history, &edit) != UMI_STATUS_OK ||
        umi_editor_edit_history_undo_count(history) != 1U) return 2;
    if (umi_editor_edit_history_undo(history, &operation) != UMI_STATUS_OK ||
        operation.removed_byte_count != 4U || operation.inserted_byte_count != 3U ||
        memcmp(operation.inserted_bytes, "old", 3U) != 0) return 3;
    if (umi_editor_edit_history_redo_count(history) != 1U) return 4;
    if (umi_editor_edit_history_redo(history, &operation) != UMI_STATUS_OK ||
        operation.operation_id != 7U) return 5;
    umi_editor_edit_history_destroy(history);
    return EXIT_SUCCESS;
}
