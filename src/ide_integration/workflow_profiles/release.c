/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_profiles/release.c
 *
 * PURPOSE:
 *   Release readiness requires clean source control and complete test/diagnostic/debug/AI gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_profiles/release.h"

/*
 * Release or reset state held by ide workflow profile so the same storage can be reused
 * safely.
 */
UmiStatus umi_ide_workflow_profile_release(
    UmiIdeWorkflowPolicy *out_policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ide_workflow_policy_init(out_policy);
    out_policy->require_document = 0;
    out_policy->allow_dirty_document = 0;
    out_policy->require_tests = 1;
    out_policy->require_all_tests_passed = 1;
    out_policy->require_source_control = 1;
    out_policy->require_clean_source_control = 1;
    out_policy->require_debugger_idle = 1;
    out_policy->require_no_pending_ai_approval = 1;
    out_policy->require_ai_idle = 1;
    out_policy->revision += 1U;
    return UMI_STATUS_OK;
}
