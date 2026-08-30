/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_health.c
 *
 * PURPOSE:
 *   Implement project inspection service state into a simple healthy/degraded/error code.
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

#include "umicom/repository/inspection_health.h"

/* Project inspection service state into a simple healthy/degraded/error code. */
int umi_repository_inspection_health_code(
    const UmiRepositoryInspectionService *service)
{
    /* A missing service cannot provide health and is reported separately. */
    if (service == NULL) {
        return -1;
    }
    /* Probe or orchestration failure is an error even without typed issues. */
    if (service->last_status != UMI_STATUS_OK) {
        return 2;
    }
    /* Typed error issues take precedence over warning degradation. */
    if (service->maintenance.report.errors > 0U || service->report.errors > 0U) {
        return 2;
    }
    if (service->maintenance.report.warnings > 0U || service->report.warnings > 0U) {
        return 1;
    }
    return 0;
}
