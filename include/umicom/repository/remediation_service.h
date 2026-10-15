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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the repository remediation service data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryRemediationService { UmiRepositoryInspectionService inspection; UmiRepositoryRemediationPolicy policy; UmiRepositoryRemediationPlan plan; UmiRepositoryRemediationReport report; UmiStatus last_status; uint64_t revision; } UmiRepositoryRemediationService;
/* Initialise the service with conservative execution policy. */
void umi_repository_remediation_service_init(UmiRepositoryRemediationService *service);
/* Refresh inspection and build a plan; this function never applies the plan. */
/** Publish new evidence and a proposal together; preserve both on failure.
 * last_status describes the latest attempt. A failed attempt makes the retained
 * plan stale. One owner must serialize access; no disk transaction is implied. */
UmiStatus umi_repository_remediation_service_plan(UmiRepositoryRemediationService *service,const UmiRepositoryInspectionContext *context);
/** Return whether a plan and its observation belong to successful refreshes.
 * This is evidence freshness, not permission to execute a proposed action. */
int UmiRepositoryRemediationEvidenceCurrent(const UmiRepositoryRemediationService *service);
#ifdef __cplusplus
}
#endif
#endif
