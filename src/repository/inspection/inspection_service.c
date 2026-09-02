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
