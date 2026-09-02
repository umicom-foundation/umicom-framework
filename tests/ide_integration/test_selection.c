/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_selection.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/selection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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

