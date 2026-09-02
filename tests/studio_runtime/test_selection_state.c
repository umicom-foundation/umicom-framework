/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_selection_state.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime selection state.
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
#include "umicom/studio_runtime/selection_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeSelectionState state;
    UmiStudioRuntimeSelection selection = {0};
    umi_studio_selection_state_init(&state);
    selection.kind = UMI_STUDIO_SELECTION_TEST;
    (void)strcpy(selection.subject_id,"test.a");
    assert(umi_studio_selection_state_set(&state,&selection)==UMI_STATUS_OK);
    assert(state.current.kind == UMI_STUDIO_SELECTION_TEST);
    umi_studio_selection_state_clear(&state);
    assert(state.current.kind == UMI_STUDIO_SELECTION_NONE);
    return 0;
}

