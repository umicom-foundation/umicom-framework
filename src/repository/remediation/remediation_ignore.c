/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_ignore.c
 *
 * PURPOSE:
 *   Implement create a safe proposal for a missing root ignore policy.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_ignore.h"
/* Construct a plan-only action; command_preview is explanatory and is not executed here. */
UmiStatus umi_repository_remediation_ignore_plan(UmiRepositoryRemediationAction *out_action)
{
    return umi_repository_remediation_action_set(out_action,UMI_REMEDIATION_ADD_GITIGNORE,UMI_REPOSITORY_REMEDIATION_REVIEW,"Add reviewed .gitignore policy","Build outputs should not become accidental source artifacts.","edit .gitignore",0);
}
