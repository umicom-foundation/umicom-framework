/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_upstream.c
 *
 * PURPOSE:
 *   Implement create a safe configuration plan when upstream metadata is absent.
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

#include "umicom/repository/remediation_upstream.h"
/* Construct a plan-only action; command_preview is explanatory and is not executed here. */
UmiStatus umi_repository_remediation_upstream_plan(UmiRepositoryRemediationAction *out_action)
{
    return umi_repository_remediation_action_set(out_action,UMI_REMEDIATION_CONFIGURE_UPSTREAM,UMI_REPOSITORY_REMEDIATION_SAFE,"Configure branch upstream","The current branch has no upstream; remote comparison is incomplete.","git branch --set-upstream-to origin/<branch>",0);
}
