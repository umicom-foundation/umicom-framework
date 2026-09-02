/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_types.c
 *
 * PURPOSE:
 *   Verify provider enum names, bounded copying and hashing.
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

#include "umicom/workbench_selection_provider/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[32U];
    assert(umi_workbench_selection_provider_copy_text(
        text, sizeof(text), "project") == UMI_STATUS_OK);
    assert(strcmp(text, "project") == 0);
    assert(strcmp(umi_workbench_selection_provider_kind_text(
        UMI_WORKBENCH_SELECTION_PROVIDER_PROBLEM), "problem") == 0);
    assert(strcmp(umi_workbench_selection_provider_state_text(
        UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE), "active") == 0);

    return 0;
}
