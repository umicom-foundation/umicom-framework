/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_action_set.h
 *
 * PURPOSE:
 *   Maintain a bounded ordered set of proposed remediation actions.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ACTION_SET_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ACTION_SET_H
#include <stddef.h>
#include "umicom/repository/remediation_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/remediation_action.h"
typedef struct UmiRepositoryRemediationActionSet { UmiRepositoryRemediationAction items[UMI_REPOSITORY_REMEDIATION_ACTION_CAPACITY]; size_t count; } UmiRepositoryRemediationActionSet;
/* Initialise an empty remediation action set. */
void umi_repository_remediation_action_set_init(UmiRepositoryRemediationActionSet *set);
/* Append one reviewed action without reordering existing proposals. */
UmiStatus umi_repository_remediation_action_set_add(UmiRepositoryRemediationActionSet *set,const UmiRepositoryRemediationAction *action);
#ifdef __cplusplus
}
#endif
#endif
