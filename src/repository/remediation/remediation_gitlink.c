/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_gitlink.c
 *
 * PURPOSE:
 *   Implement create a review plan for .gitmodules/gitlink alignment issues.
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

#include "umicom/repository/remediation_gitlink.h"
/* Construct a plan-only action; command_preview is explanatory and is not executed here. */
UmiStatus umi_repository_remediation_gitlink_plan(UmiRepositoryRemediationAction *out_action)
{
    return umi_repository_remediation_action_set(out_action,UMI_REMEDIATION_REVIEW_GITLINKS,UMI_REPOSITORY_REMEDIATION_REVIEW,"Review parent gitlinks","Configured submodules and tracked gitlinks are not aligned.","git ls-files --stage",0);
}
