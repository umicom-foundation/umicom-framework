/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_profiles/pre_commit.c
 *
 * PURPOSE:
 *   Pre-commit requires a workspace, no errors/fatals, tests and source control, but permits staged/unstaged changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_profiles/pre_commit.h"

UmiStatus umi_ide_workflow_profile_pre_commit(
    UmiIdeWorkflowPolicy *out_policy)
{
    if (out_policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ide_workflow_policy_init(out_policy);
    out_policy->require_document = 0;
    out_policy->allow_dirty_document = 0;
    out_policy->require_tests = 1;
    out_policy->require_all_tests_passed = 1;
    out_policy->require_source_control = 1;
    out_policy->require_clean_source_control = 0;
    out_policy->require_debugger_idle = 1;
    out_policy->require_no_pending_ai_approval = 1;
    out_policy->revision += 1U;
    return UMI_STATUS_OK;
}
