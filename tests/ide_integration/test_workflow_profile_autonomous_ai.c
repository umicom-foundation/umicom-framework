/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_profile_autonomous_ai.c
 *
 * PURPOSE:
 *   Verify the reusable autonomous ai IDE readiness policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ide_integration/workflow_profiles/autonomous_ai.h"

int main(void)
{
    UmiIdeWorkflowPolicy policy;
    assert(umi_ide_workflow_profile_autonomous_ai(&policy) == UMI_STATUS_OK);
    assert(policy.require_workspace);
    assert(policy.revision >= 2U);
    return 0;
}
