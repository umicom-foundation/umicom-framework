/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_evaluator.c
 *
 * PURPOSE:
 *   Integration regression coverage for workflow evaluator.
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
#include "umicom/ide_integration/workflow_evaluator.h"
int main(void)
{
    UmiIdeIntegrationBindings bindings;
    UmiIdeActiveContext context;
    UmiIdeWorkflowPolicy policy;
    UmiIdeWorkflowReport report;
    umi_ide_integration_bindings_init(&bindings);
    (void)memset(&context, 0, sizeof(context));
    (void)strcpy(context.workspace_root, ".");
    context.has_problems = 1;
    context.problems.errors = 1U;
    context.problems.count = 1U;
    umi_ide_workflow_policy_init(&policy);
    policy.require_tests = 0;
    policy.require_source_control = 0;
    policy.require_debugger_idle = 0;
    policy.require_no_pending_ai_approval = 0;
    assert(umi_ide_workflow_evaluate(
        &bindings, &context, &policy, &report) == UMI_STATUS_OK);
    assert(!report.ready);
    assert(report.blocked_count >= 1U);
    return 0;
}

