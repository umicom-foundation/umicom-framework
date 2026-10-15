/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_service.c
 *
 * PURPOSE:
 *   Implement provide one Framework-owned read-only entry point for repository snapshot and findings.
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

#include "umicom/repository/inspection_service.h"

#include <string.h>
#include <stdlib.h>

/* Initialise composed canonical-doctor and structural-inspection state. */
void umi_repository_inspection_service_init(
    UmiRepositoryInspectionService *service)
{
    /* Optional service storage may be omitted by a caller. */
    if (service == NULL) {
        return;
    }

    /* Reuse existing Framework maintenance/doctor initialisation verbatim. */
    (void)memset(service, 0, sizeof(*service));
    umi_repository_inspection_snapshot_init(&service->snapshot);
    umi_repository_maintenance_service_init(&service->maintenance);
    /* Umicom repository inspection opts into existing remote/upstream doctor rules. */
    service->maintenance.policy.require_origin = 1;
    service->maintenance.policy.require_upstream = 1;
    umi_repository_inspection_policy_init(&service->policy);
    service->last_status = UMI_STATUS_OK;
}

/* Refresh repository evidence without duplicating doctor acceptance logic. */
/* The earlier in-place refresh is retained for review. A later probe failure
 * could leave a new partial snapshot beside an older doctor report. The
 * replacement prepares the whole projection before publishing it to observers. */
#if 0
UmiStatus umi_repository_inspection_service_refresh(
    UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context)
{
    UmiStatus status;

    /* Both the service and immutable operation context are required. */
    if (service == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Collect Git evidence once and hand canonical status to MaintenanceService. */
    status = umi_repository_snapshot_collect(context, &service->snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        service->maintenance.summary = service->snapshot.maintenance;
        status = umi_repository_maintenance_service_evaluate(
            &service->maintenance);
    }

    /* Evaluate only the structural checks not represented by DoctorPolicy. */
    if (status == UMI_STATUS_OK) {
        status = umi_repository_inspection_report_build(
            &service->snapshot, &service->policy, &service->report);
    }

    /* Preserve operation status and publish revisions only for complete refreshes. */
    service->last_status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        ++service->revision;
    }
    return status;
}

#endif
UmiStatus umi_repository_inspection_service_refresh(
    UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (context == NULL) {
        service->last_status = UMI_STATUS_INVALID_ARGUMENT;
        return service->last_status;
    }
    /* Revisions identify accepted evidence. Exhaustion cannot wrap to a value
     * that a host may mistake for an older or uninitialised snapshot. */
    if (service->revision == UINT64_MAX || service->maintenance.revision == UINT64_MAX) {
        service->last_status = UMI_STATUS_CAPACITY_EXCEEDED;
        return service->last_status;
    }
    UmiRepositoryInspectionService *candidate = malloc(sizeof *candidate);
    if (candidate == NULL) {
        service->last_status = UMI_STATUS_OUT_OF_MEMORY;
        return service->last_status;
    }
    /* Copy policies and the previous revision into heap storage. The reports
     * are sizeable value objects and must not consume a UI worker's stack. */
    *candidate = *service;
    UmiStatus status = umi_repository_snapshot_collect(context, &candidate->snapshot);
    if (status == UMI_STATUS_OK) {
        candidate->maintenance.summary = candidate->snapshot.maintenance;
        status = umi_repository_maintenance_service_evaluate(&candidate->maintenance);
    }
    if (status == UMI_STATUS_OK)
        status = umi_repository_inspection_report_build(
            &candidate->snapshot, &candidate->policy, &candidate->report);
    if (status == UMI_STATUS_OK) {
        candidate->last_status = UMI_STATUS_OK;
        candidate->revision = service->revision + 1U;
        candidate->snapshot.revision = candidate->revision;
        *service = *candidate;
    } else {
        /* Retain the last complete evidence for display, but mark it stale
         * through last_status. Failed evidence is never a new accepted revision. */
        service->last_status = status;
    }
    free(candidate);
    return status;
}

/* A zero revision is initial state, not an observation of a clean repository.
 * Keep this rule shared by diagnostics, status text and remediation consumers. */
UmiRepositoryInspectionEvidence UmiRepositoryInspectionEvidenceRead(
    const UmiRepositoryInspectionService *service)
{
    if (service == NULL || service->revision == 0U)
        return UMI_REPOSITORY_INSPECTION_NONE;
    if (service->last_status != UMI_STATUS_OK ||
        service->snapshot.revision != service->revision)
        return UMI_REPOSITORY_INSPECTION_STALE;
    return UMI_REPOSITORY_INSPECTION_CURRENT;
}

const char *UmiRepositoryInspectionEvidenceText(UmiRepositoryInspectionEvidence evidence)
{
    /* Labels describe observed evidence rather than claiming repository health. */
    switch (evidence) {
    case UMI_REPOSITORY_INSPECTION_NONE: return "No successful observation";
    case UMI_REPOSITORY_INSPECTION_CURRENT: return "Last refresh succeeded";
    case UMI_REPOSITORY_INSPECTION_STALE: return "Stale retained observation";
    default: return "Unknown observation state";
    }
}
