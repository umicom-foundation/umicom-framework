/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_types.c
 *
 * PURPOSE:
 *   Verify stable workbench lifecycle and review text contracts.
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

#include "umicom/vcs/workbench/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[8];
    assert(strcmp(umi_vcs_workbench_phase_text(
                      UMI_VCS_WORKBENCH_MERGING), "merging") == 0);
    assert(strcmp(umi_vcs_workbench_review_decision_text(
                      UMI_VCS_WORKBENCH_REVIEW_APPROVED), "approved") == 0);
    assert(umi_vcs_workbench_copy_text(text, sizeof(text), "ready") ==
           UMI_STATUS_OK);
    assert(strcmp(text, "ready") == 0);
    assert(umi_vcs_workbench_copy_text(text, sizeof(text), "too-long") ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
