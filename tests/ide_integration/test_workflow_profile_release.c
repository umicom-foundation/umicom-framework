/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_profile_release.c
 *
 * PURPOSE:
 *   Verify the reusable release IDE readiness policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ide_integration/workflow_profiles/release.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIdeWorkflowPolicy policy;
    assert(umi_ide_workflow_profile_release(&policy) == UMI_STATUS_OK);
    assert(policy.require_workspace);
    assert(policy.revision >= 2U);
    return 0;
}
