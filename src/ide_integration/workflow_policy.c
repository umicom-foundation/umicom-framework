/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_policy.c
 *
 * PURPOSE:
 *   Initialize conservative professional workflow policy defaults.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_policy.h"

#include <string.h>

void umi_ide_workflow_policy_init(UmiIdeWorkflowPolicy *policy)
{
    if (policy == NULL) return;

    (void)memset(policy, 0, sizeof(*policy));
    policy->require_workspace = 1;
    policy->allow_dirty_document = 1;
    policy->require_no_errors = 1;
    policy->require_no_fatals = 1;
    policy->require_tests = 1;
    policy->require_all_tests_passed = 1;
    policy->require_source_control = 1;
    policy->require_debugger_idle = 1;
    policy->require_no_pending_ai_approval = 1;
    policy->require_ai_idle = 0;
    policy->revision = 1U;
}
