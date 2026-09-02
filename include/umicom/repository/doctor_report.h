/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/doctor_report.h
 *
 * PURPOSE:
 *   Represent repository-doctor outcome, counts and retained findings.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_REPORT_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_REPORT_H
#include "umicom/repository/status_summary.h"
#include "umicom/repository/doctor_issue.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository doctor report data shared with callers of this public contract.
 */
typedef struct UmiRepositoryDoctorReport {
    UmiRepositoryStatusSummary summary;
    UmiRepositoryDoctorIssueList issues;
    size_t warnings;
    size_t errors;
    int healthy;
} UmiRepositoryDoctorReport;

/**
 * Initialise repository doctor report from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_doctor_report_init(UmiRepositoryDoctorReport *report);
/**
 * Provide the repository doctor report finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_doctor_report_finalize(UmiRepositoryDoctorReport *report);

#ifdef __cplusplus
}
#endif
#endif
