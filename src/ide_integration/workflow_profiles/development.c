/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_profiles/development.c
 *
 * PURPOSE:
 *   Interactive development permits dirty files and active debug sessions while surfacing warnings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_profiles/development.h"

/*
 * Provide the ide workflow profile development operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_workflow_profile_development(
    UmiIdeWorkflowPolicy *out_policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ide_workflow_policy_init(out_policy);
    out_policy->require_document = 0;
    out_policy->allow_dirty_document = 1;
    out_policy->require_tests = 0;
    out_policy->require_all_tests_passed = 0;
    out_policy->require_source_control = 0;
    out_policy->require_clean_source_control = 0;
    out_policy->require_debugger_idle = 0;
    out_policy->require_no_pending_ai_approval = 0;
    out_policy->revision += 1U;
    return UMI_STATUS_OK;
}
