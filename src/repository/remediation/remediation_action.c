/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_action.c
 *
 * PURPOSE:
 *   Implement represent one proposed action with explicit risk and a command preview, never an implicit mutation.
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

#include "umicom/repository/remediation_action.h"
#include <string.h>
#include "umicom/repository/inspection_types.h"
/* Store one proposal; destructive actions are always forced to explicit-review mode. */
UmiStatus umi_repository_remediation_action_set(UmiRepositoryRemediationAction *action,UmiRepositoryRemediationKind kind,UmiRepositoryRemediationRisk risk,const char *title,const char *rationale,const char *command_preview,int executable_automatically){UmiStatus s;if(!action||!title||!rationale||!command_preview)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(action,0,sizeof(*action));action->kind=kind;action->risk=risk;action->executable_automatically=(risk==UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE)?0:executable_automatically;s=umi_repository_inspection_copy_text(action->title,sizeof(action->title),title);if(s!=UMI_STATUS_OK)return s;s=umi_repository_inspection_copy_text(action->rationale,sizeof(action->rationale),rationale);if(s!=UMI_STATUS_OK)return s;return umi_repository_inspection_copy_text(action->command_preview,sizeof(action->command_preview),command_preview);}
