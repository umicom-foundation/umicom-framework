/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_gitlink.h
 *
 * PURPOSE:
 *   Create a review plan for .gitmodules/gitlink alignment issues.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_GITLINK_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_GITLINK_H
#include "umicom/repository/remediation_action.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Build this focused plan action without applying repository changes. */
UmiStatus umi_repository_remediation_gitlink_plan(UmiRepositoryRemediationAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
