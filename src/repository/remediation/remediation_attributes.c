/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_attributes.c
 *
 * PURPOSE:
 *   Implement create a safe proposal for a missing root attribute policy.
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

#include "umicom/repository/remediation_attributes.h"
/* Construct a plan-only action; command_preview is explanatory and is not executed here. */
UmiStatus umi_repository_remediation_attributes_plan(UmiRepositoryRemediationAction *out_action)
{
    return umi_repository_remediation_action_set(out_action,UMI_REMEDIATION_ADD_GITATTRIBUTES,UMI_REPOSITORY_REMEDIATION_REVIEW,"Add reviewed .gitattributes policy","Line-ending and binary file behaviour should be explicit.","edit .gitattributes",0);
}
