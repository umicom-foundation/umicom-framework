/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_selection.c
 *
 * PURPOSE:
 *   Verify canonical selection origin, location, fields, validation and hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/selection.h"

int main(void)
{
    UmiWorkbenchSelection selection;
    UmiWorkbenchSelectionField field;
    uint64_t first_hash;

    umi_workbench_selection_init(
        &selection,
        UMI_WORKBENCH_SELECTION_FILE,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        "selection");
    assert(umi_workbench_selection_set_origin(
        &selection, "studio", "studio.editor",
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_selection_set_subject(
        &selection, "main.c", "main") == UMI_STATUS_OK);
    assert(umi_workbench_selection_set_location(
        &selection, "C:/dev/main.c",
        20U, 8U, 4U) == UMI_STATUS_OK);

    umi_workbench_selection_field_init(&field, "language-id");
    assert(umi_workbench_selection_field_set_text(
        &field, "c") == UMI_STATUS_OK);
    assert(umi_workbench_selection_add_field(
        &selection, &field) == UMI_STATUS_OK);

    assert(umi_workbench_selection_validate(
        &selection) == UMI_STATUS_OK);
    first_hash = umi_workbench_selection_refresh_hash(&selection);
    assert(first_hash != 0U);
    selection.line = 21U;
    assert(umi_workbench_selection_refresh_hash(&selection) != first_hash);
    assert(strcmp(selection.path, "C:/dev/main.c") == 0);
    return 0;
}
