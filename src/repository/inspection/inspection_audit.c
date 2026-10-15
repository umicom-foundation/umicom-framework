/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_audit.c
 *
 * PURPOSE:
 *   Implement combine existing doctor report semantics with the richer inspection report.
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

#include "umicom/repository/inspection_audit.h"

/* Report readiness only when collection succeeded and neither report has errors. */
int umi_repository_inspection_audit_ready(
    const UmiRepositoryInspectionService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    /* Preserve the former status-only guard for review. Initial state has an
     * OK status too; the replacement also requires an accepted observation. */
#if 0
    if (service == NULL || service->last_status != UMI_STATUS_OK) {
        return 0;
    }
#endif
    if (UmiRepositoryInspectionEvidenceRead(service) != UMI_REPOSITORY_INSPECTION_CURRENT)
        return 0;
    return service->maintenance.report.errors == 0U && service->report.errors == 0U;
}
