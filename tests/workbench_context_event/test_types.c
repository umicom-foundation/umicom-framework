/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_types.c
 *
 * PURPOSE:
 *   Verify bounded text and event-kind text conversion.
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

#include "umicom/workbench_context_event/types.h"

int main(void)
{
    char text[16U];
    assert(umi_workbench_context_event_copy_text(
        text, sizeof(text), "editor") == UMI_STATUS_OK);
    assert(strcmp(text, "editor") == 0);
    assert(umi_workbench_context_event_copy_text(
        text, 4U, "editor") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(umi_workbench_context_event_kind_text(
        UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET),
        "editor-caret") == 0);

    return 0;
}
