/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_types.c
 *
 * PURPOSE:
 *   Verify source kind, trigger and bounded text helpers.
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

#include "umicom/workbench_context_source/types.h"

int main(void)
{
    char text[16U];
    assert(umi_workbench_context_source_copy_text(
        text, sizeof(text), "editor") == UMI_STATUS_OK);
    assert(strcmp(text, "editor") == 0);
    assert(strcmp(
        umi_workbench_context_source_kind_text(
            UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR),
        "editor") == 0);
    assert(strcmp(
        umi_workbench_context_source_trigger_text(
            UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET),
        "caret") == 0);

    return 0;
}
