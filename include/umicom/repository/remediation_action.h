/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_action.h
 *
 * PURPOSE:
 *   Represent one proposed action with explicit risk and a command preview, never an implicit mutation.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ACTION_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ACTION_H
#include <stddef.h>
#include "umicom/repository/remediation_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryRemediationAction { UmiRepositoryRemediationKind kind; UmiRepositoryRemediationRisk risk; char title[UMI_REPOSITORY_REMEDIATION_TEXT_CAPACITY]; char rationale[UMI_REPOSITORY_REMEDIATION_TEXT_CAPACITY]; char command_preview[UMI_REPOSITORY_REMEDIATION_TEXT_CAPACITY]; int executable_automatically; } UmiRepositoryRemediationAction;
/* Initialise one remediation proposal with bounded explanatory text. */
UmiStatus umi_repository_remediation_action_set(UmiRepositoryRemediationAction *action,UmiRepositoryRemediationKind kind,UmiRepositoryRemediationRisk risk,const char *title,const char *rationale,const char *command_preview,int executable_automatically);
#ifdef __cplusplus
}
#endif
#endif
