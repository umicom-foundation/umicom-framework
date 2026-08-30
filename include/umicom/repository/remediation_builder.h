/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_builder.h
 *
 * PURPOSE:
 *   Translate inspection findings into non-mutating remediation proposals.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_BUILDER_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_BUILDER_H

#include "umicom/repository/doctor_report.h"
#include "umicom/repository/inspection_report.h"
#include "umicom/repository/remediation_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Translate canonical doctor findings plus structural extensions into one review plan. */
UmiStatus umi_repository_remediation_builder_build(
    const UmiRepositoryDoctorReport *doctor_report,
    const UmiRepositoryInspectionReport *inspection_report,
    UmiRepositoryRemediationPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
