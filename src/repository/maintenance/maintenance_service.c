/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/maintenance_service.c
 *
 * PURPOSE:
 *   Implement Framework-owned repository maintenance orchestration.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/repository/maintenance_service.h"

#include <string.h>

/*
 * Initialise repository maintenance service from caller-provided values so later
 * operations receive a known state.
 */
void umi_repository_maintenance_service_init(UmiRepositoryMaintenanceService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    umi_repository_status_summary_init(&service->summary);
    umi_repository_doctor_policy_default(&service->policy);
    umi_repository_doctor_report_init(&service->report);
    service->last_status = UMI_STATUS_OK;
    service->revision = 1U;
}

/*
 * Provide the repository maintenance service evaluate operation used by this module and
 * its client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_repository_maintenance_service_evaluate(UmiRepositoryMaintenanceService *service)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_status_summary_refresh(&service->summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_repository_doctor_evaluate(&service->summary, &service->policy, &service->report);
    }
    service->last_status = status;
    service->revision += 1U;
    return status;
}
#endif

UmiStatus umi_repository_maintenance_service_evaluate(UmiRepositoryMaintenanceService *service)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (service != NULL && service->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_status_summary_refresh(&service->summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_repository_doctor_evaluate(&service->summary, &service->policy, &service->report);
    }
    service->last_status = status;
    service->revision += 1U;
    return status;
}
