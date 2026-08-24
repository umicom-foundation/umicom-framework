/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_selection.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/selection.h"
int main(void)
{
    UmiIdeEditorSelection selection;
    umi_ide_editor_selection_init(&selection);
    assert(umi_ide_editor_selection_set(
        &selection, "doc", "src/main.c", "c", "int value",
        2U, 1U, 2U, 10U, 5U, 1) == UMI_STATUS_OK);
    assert(strcmp(selection.language_id, "c") == 0);
    assert(selection.dirty);
    return 0;
}

