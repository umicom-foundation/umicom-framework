/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration workflow policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/workflow_policy.h"
int main(void)
{
    UmiIdeWorkflowPolicy policy;
    umi_ide_workflow_policy_init(&policy);
    assert(policy.require_workspace);
    assert(policy.require_tests);
    assert(policy.require_no_errors);
    assert(policy.require_debugger_idle);
    return 0;
}

