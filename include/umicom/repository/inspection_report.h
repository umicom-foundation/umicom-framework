/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_report.h
 *
 * PURPOSE:
 *   Evaluate additive repository inspection rules while preserving the established doctor report.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_REPORT_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/repository/inspection_issue.h"
#include "umicom/repository/inspection_policy.h"
typedef struct UmiRepositoryInspectionReport { UmiRepositoryInspectionIssue issues[UMI_REPOSITORY_INSPECTION_ISSUE_CAPACITY]; size_t count; size_t warnings; size_t errors; } UmiRepositoryInspectionReport;
/* Evaluate the collected snapshot against additive inspection policy. */
UmiStatus umi_repository_inspection_report_build(const UmiRepositoryInspectionSnapshot *snapshot, const UmiRepositoryInspectionPolicy *policy, UmiRepositoryInspectionReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
