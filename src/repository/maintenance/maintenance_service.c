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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/maintenance_service.h"

#include <string.h>

void umi_repository_maintenance_service_init(UmiRepositoryMaintenanceService *service)
{
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    umi_repository_status_summary_init(&service->summary);
    umi_repository_doctor_policy_default(&service->policy);
    umi_repository_doctor_report_init(&service->report);
    service->last_status = UMI_STATUS_OK;
    service->revision = 1U;
}

UmiStatus umi_repository_maintenance_service_evaluate(UmiRepositoryMaintenanceService *service)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_status_summary_refresh(&service->summary);
    if (status == UMI_STATUS_OK) {
        status = umi_repository_doctor_evaluate(&service->summary, &service->policy, &service->report);
    }
    service->last_status = status;
    service->revision += 1U;
    return status;
}
