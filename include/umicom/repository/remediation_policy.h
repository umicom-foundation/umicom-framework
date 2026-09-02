/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_policy.h
 *
 * PURPOSE:
 *   Define what repository repairs may be suggested or automatically executed.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_POLICY_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_POLICY_H
#include <stddef.h>
#include "umicom/repository/remediation_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository remediation policy data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryRemediationPolicy { int allow_safe_automatic; int allow_review_automatic; int allow_destructive_automatic; } UmiRepositoryRemediationPolicy;
/* Initialise conservative policy that never automates review/destructive actions. */
void umi_repository_remediation_policy_init(UmiRepositoryRemediationPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
