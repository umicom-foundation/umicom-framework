/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_upstream.h
 *
 * PURPOSE:
 *   Create a safe configuration plan when upstream metadata is absent.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_UPSTREAM_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_UPSTREAM_H
#include "umicom/repository/remediation_action.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Build this focused plan action without applying repository changes. */
UmiStatus umi_repository_remediation_upstream_plan(UmiRepositoryRemediationAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
