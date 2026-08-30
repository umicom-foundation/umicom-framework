/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_action_set.c
 *
 * PURPOSE:
 *   Implement maintain a bounded ordered set of proposed remediation actions.
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

#include "umicom/repository/remediation_action_set.h"
#include <string.h>
/* Initialise a deterministic empty proposal collection. */
void umi_repository_remediation_action_set_init(UmiRepositoryRemediationActionSet *set){if(set)(void)memset(set,0,sizeof(*set));}
/* Append one proposal while preserving caller-supplied ordering. */
UmiStatus umi_repository_remediation_action_set_add(UmiRepositoryRemediationActionSet *set,const UmiRepositoryRemediationAction *action){if(!set||!action)return UMI_STATUS_INVALID_ARGUMENT;if(set->count>=UMI_REPOSITORY_REMEDIATION_ACTION_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;set->items[set->count++]=*action;return UMI_STATUS_OK;}
