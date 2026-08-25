/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_submodule.h
 *
 * PURPOSE:
 *   Create explicit plans for missing or mismatched submodules.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SUBMODULE_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SUBMODULE_H
#include "umicom/repository/remediation_action.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Build this focused plan action without applying repository changes. */
UmiStatus umi_repository_remediation_submodule_plan(UmiRepositoryRemediationAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
