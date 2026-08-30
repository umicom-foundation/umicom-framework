/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_profile_development.c
 *
 * PURPOSE:
 *   Verify the reusable development IDE readiness policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/ide_integration/workflow_profiles/development.h"

int main(void)
{
    UmiIdeWorkflowPolicy policy;
    assert(umi_ide_workflow_profile_development(&policy) == UMI_STATUS_OK);
    assert(policy.require_workspace);
    assert(policy.revision >= 2U);
    return 0;
}
