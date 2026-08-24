/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_profile_pre_commit.c
 *
 * PURPOSE:
 *   Verify the reusable pre commit IDE readiness policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ide_integration/workflow_profiles/pre_commit.h"

int main(void)
{
    UmiIdeWorkflowPolicy policy;
    assert(umi_ide_workflow_profile_pre_commit(&policy) == UMI_STATUS_OK);
    assert(policy.require_workspace);
    assert(policy.revision >= 2U);
    return 0;
}
