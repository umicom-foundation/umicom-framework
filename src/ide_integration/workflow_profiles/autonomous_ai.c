/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_profiles/autonomous_ai.c
 *
 * PURPOSE:
 *   Autonomous AI repair permits dirty working state but blocks unresolved approvals and concurrent debug control.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_profiles/autonomous_ai.h"

UmiStatus umi_ide_workflow_profile_autonomous_ai(
    UmiIdeWorkflowPolicy *out_policy)
{
    if (out_policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ide_workflow_policy_init(out_policy);
    out_policy->require_document = 0;
    out_policy->allow_dirty_document = 1;
    out_policy->require_tests = 0;
    out_policy->require_all_tests_passed = 0;
    out_policy->require_source_control = 1;
    out_policy->require_clean_source_control = 0;
    out_policy->require_debugger_idle = 1;
    out_policy->require_no_pending_ai_approval = 1;
    out_policy->require_ai_idle = 0;
    out_policy->revision += 1U;
    return UMI_STATUS_OK;
}
