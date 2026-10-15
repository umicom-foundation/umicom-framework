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

/**
 * Represent the repository inspection service data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryInspectionService {
    UmiRepositoryInspectionSnapshot snapshot;
    UmiRepositoryMaintenanceService maintenance;
    UmiRepositoryInspectionPolicy policy;
    UmiRepositoryInspectionReport report;
    UmiStatus last_status;
    uint64_t revision;
} UmiRepositoryInspectionService;

/** Distinguish collected evidence from empty or retained observations.
 * CURRENT means the last refresh succeeded; it does not promise that files
 * have stayed unchanged since then or that a remote has been contacted. */
typedef enum UmiRepositoryInspectionEvidence {
    UMI_REPOSITORY_INSPECTION_NONE,
    UMI_REPOSITORY_INSPECTION_CURRENT,
    UMI_REPOSITORY_INSPECTION_STALE
} UmiRepositoryInspectionEvidence;

/** Read evidence availability without performing Git operations.
 * A null or never-refreshed service has NONE. Retained evidence is STALE after
 * a failed refresh or when its accepted revision no longer matches the service. */
UmiRepositoryInspectionEvidence UmiRepositoryInspectionEvidenceRead(
    const UmiRepositoryInspectionService *service);

/** Return a stable human-readable description; the returned string is borrowed. */
const char *UmiRepositoryInspectionEvidenceText(UmiRepositoryInspectionEvidence evidence);

/* Initialise both canonical doctor state and additive structural inspection state. */
void umi_repository_inspection_service_init(
    UmiRepositoryInspectionService *service);

/* Refresh Git evidence, evaluate the existing doctor, then evaluate structural checks. */
/** Publish a complete refresh only after collection and both evaluations succeed.
 * On failure, retain the previous snapshot, reports, policies and revisions,
 * and set last_status to the failure. Hosts must show that evidence as stale.
 * Call from one owning worker with no concurrent access to service. This guards
 * in-memory publication, not a transaction over Git or files on disk.
 * Counter exhaustion fails before any probes run; no revision wraps to zero. */
UmiStatus umi_repository_inspection_service_refresh(
    UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context);

#ifdef __cplusplus
}
#endif
#endif
