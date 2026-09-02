/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_field.c
 *
 * PURPOSE:
 *   Verify typed structured-selection fields.
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

#include "umicom/workbench_selection/field.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchSelectionField field;
    umi_workbench_selection_field_init(&field, "branch");
    assert(umi_workbench_selection_field_set_text(
        &field, "main") == UMI_STATUS_OK);
    assert(umi_workbench_selection_field_validate(
        &field) == UMI_STATUS_OK);
    assert(field.kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT);
    assert(strcmp(field.text, "main") == 0);

    umi_workbench_selection_field_init(&field, "duration-ms");
    assert(umi_workbench_selection_field_set_unsigned(
        &field, 25U) == UMI_STATUS_OK);
    assert(field.unsigned_value == 25U);
    return 0;
}
