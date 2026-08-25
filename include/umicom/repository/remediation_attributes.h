/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_attributes.h
 *
 * PURPOSE:
 *   Create a safe proposal for a missing root attribute policy.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ATTRIBUTES_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_ATTRIBUTES_H
#include "umicom/repository/remediation_action.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Build this focused plan action without applying repository changes. */
UmiStatus umi_repository_remediation_attributes_plan(UmiRepositoryRemediationAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
