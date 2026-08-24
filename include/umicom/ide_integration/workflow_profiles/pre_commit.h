/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/pre_commit.h
 *
 * PURPOSE:
 *   Pre-commit requires a workspace, no errors/fatals, tests and source control, but permits staged/unstaged changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_PRE_COMMIT_H
#define UMICOM_IDE_WORKFLOW_PROFILE_PRE_COMMIT_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_workflow_profile_pre_commit(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
