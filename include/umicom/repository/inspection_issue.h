/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_issue.h
 *
 * PURPOSE:
 *   Define bounded inspection findings without replacing the existing repository doctor issue model.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_ISSUE_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_ISSUE_H

#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Structural findings extend, but never mirror, UmiRepositoryDoctorIssue. */
typedef enum UmiRepositoryInspectionIssueKind {
    UMI_REPOSITORY_INSPECTION_FETCH_UNAVAILABLE = 1,
    UMI_REPOSITORY_INSPECTION_GITLINK_COUNT_MISMATCH = 2,
    UMI_REPOSITORY_INSPECTION_MISSING_GITIGNORE = 3,
    UMI_REPOSITORY_INSPECTION_MISSING_GITATTRIBUTES = 4
} UmiRepositoryInspectionIssueKind;

typedef struct UmiRepositoryInspectionIssue {
    UmiRepositoryInspectionIssueKind kind;
    UmiRepositoryInspectionSeverity severity;
    char summary[512];
    char remediation_hint[512];
} UmiRepositoryInspectionIssue;

/* Initialise one structural finding with typed evidence and a review hint. */
UmiStatus umi_repository_inspection_issue_set(
    UmiRepositoryInspectionIssue *issue,
    UmiRepositoryInspectionIssueKind kind,
    UmiRepositoryInspectionSeverity severity,
    const char *summary,
    const char *hint);

#ifdef __cplusplus
}
#endif
#endif
