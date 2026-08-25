/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_guard.h
 *
 * PURPOSE:
 *   Enforce non-destructive repository planning and explicit review boundaries.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_GUARD_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_GUARD_H
#include <stddef.h>
#include "umicom/repository/remediation_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/remediation_action.h"
#include "umicom/repository/remediation_policy.h"
/* Decide whether a proposed action is eligible for automatic execution under policy. */
int umi_repository_remediation_guard_allows(const UmiRepositoryRemediationPolicy *policy,const UmiRepositoryRemediationAction *action);
#ifdef __cplusplus
}
#endif
#endif
