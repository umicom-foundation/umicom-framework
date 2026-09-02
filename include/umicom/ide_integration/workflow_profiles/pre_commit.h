/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/pre_commit.h
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
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_PRE_COMMIT_H
#define UMICOM_IDE_WORKFLOW_PROFILE_PRE_COMMIT_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide workflow profile pre commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_workflow_profile_pre_commit(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
