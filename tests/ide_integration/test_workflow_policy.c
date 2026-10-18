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

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include "umicom/ide_integration/workflow_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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

