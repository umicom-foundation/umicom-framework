/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_submodule.c
 *
 * PURPOSE:
 *   Implement create explicit plans for missing or mismatched submodules.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_submodule.h"
/* Construct a plan-only action; command_preview is explanatory and is not executed here. */
UmiStatus umi_repository_remediation_submodule_plan(UmiRepositoryRemediationAction *out_action)
{
    return umi_repository_remediation_action_set(out_action,UMI_REMEDIATION_INIT_SUBMODULES,UMI_REPOSITORY_REMEDIATION_REVIEW,"Initialise configured submodules","Configured submodules should be reviewed and initialised recursively.","git submodule update --init --recursive",0);
}
