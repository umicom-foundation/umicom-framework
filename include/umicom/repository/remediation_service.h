/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_service.h
 *
 * PURPOSE:
 *   Compose inspection evidence and plan-only remediation into one Framework service.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SERVICE_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_SERVICE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/remediation_types.h"
#include "umicom/repository/remediation_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/inspection_service.h"
#include "umicom/repository/remediation_report.h"
typedef struct UmiRepositoryRemediationService { UmiRepositoryInspectionService inspection; UmiRepositoryRemediationPolicy policy; UmiRepositoryRemediationPlan plan; UmiRepositoryRemediationReport report; UmiStatus last_status; uint64_t revision; } UmiRepositoryRemediationService;
/* Initialise the service with conservative execution policy. */
void umi_repository_remediation_service_init(UmiRepositoryRemediationService *service);
/* Refresh inspection and build a plan; this function never applies the plan. */
UmiStatus umi_repository_remediation_service_plan(UmiRepositoryRemediationService *service,const UmiRepositoryInspectionContext *context);
#ifdef __cplusplus
}
#endif
#endif
