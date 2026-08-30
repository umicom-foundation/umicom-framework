/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_service.h
 *
 * PURPOSE:
 *   Provide one Framework-owned read-only entry point for repository snapshot and findings.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_SERVICE_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_SERVICE_H

#include <stdint.h>
#include "umicom/repository/maintenance_service.h"
#include "umicom/repository/repository_snapshot.h"
#include "umicom/repository/inspection_report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryInspectionService {
    UmiRepositoryInspectionSnapshot snapshot;
    UmiRepositoryMaintenanceService maintenance;
    UmiRepositoryInspectionPolicy policy;
    UmiRepositoryInspectionReport report;
    UmiStatus last_status;
    uint64_t revision;
} UmiRepositoryInspectionService;

/* Initialise both canonical doctor state and additive structural inspection state. */
void umi_repository_inspection_service_init(
    UmiRepositoryInspectionService *service);

/* Refresh Git evidence, evaluate the existing doctor, then evaluate structural checks. */
UmiStatus umi_repository_inspection_service_refresh(
    UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context);

#ifdef __cplusplus
}
#endif
#endif
