/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/doctor.h
 *
 * PURPOSE:
 *   Evaluate repository state against policy without modifying files, index or remotes.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_H
#include "umicom/repository/doctor_policy.h"
#include "umicom/repository/doctor_report.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_repository_doctor_evaluate(
    const UmiRepositoryStatusSummary *summary,
    const UmiRepositoryDoctorPolicy *policy,
    UmiRepositoryDoctorReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
