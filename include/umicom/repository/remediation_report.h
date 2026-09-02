/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_report.h
 *
 * PURPOSE:
 *   Summarise a plan without applying it.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_REPORT_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/remediation_types.h"
#include "umicom/repository/remediation_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/remediation_plan.h"
/**
 * Represent the repository remediation report data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryRemediationReport { size_t action_count; size_t automatic_eligible; size_t manual_review; int contains_destructive; } UmiRepositoryRemediationReport;
/* Summarise a plan under a supplied execution policy. */
UmiStatus umi_repository_remediation_report_build(const UmiRepositoryRemediationPlan *plan,const UmiRepositoryRemediationPolicy *policy,UmiRepositoryRemediationReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
