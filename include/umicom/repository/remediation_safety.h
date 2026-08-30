/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_safety.h
 *
 * PURPOSE:
 *   Report whether a generated plan contains any destructive-class proposal.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SAFETY_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SAFETY_H
#include <stdint.h>
#include "umicom/repository/remediation_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Report whether a generated plan contains any destructive-class proposal. */
int umi_repository_remediation_plan_safe(const UmiRepositoryRemediationPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
