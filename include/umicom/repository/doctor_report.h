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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_REPORT_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_REPORT_H
#include "umicom/repository/status_summary.h"
#include "umicom/repository/doctor_issue.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryDoctorReport {
    UmiRepositoryStatusSummary summary;
    UmiRepositoryDoctorIssueList issues;
    size_t warnings;
    size_t errors;
    int healthy;
} UmiRepositoryDoctorReport;

void umi_repository_doctor_report_init(UmiRepositoryDoctorReport *report);
UmiStatus umi_repository_doctor_report_finalize(UmiRepositoryDoctorReport *report);

#ifdef __cplusplus
}
#endif
#endif
