/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/maintenance_service.h
 *
 * PURPOSE:
 *   Compose repository state and doctor policy into a reusable Framework service.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_SERVICE_H
#define INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_SERVICE_H
#include <stdint.h>
#include "umicom/repository/doctor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryMaintenanceService {
    UmiRepositoryStatusSummary summary;
    UmiRepositoryDoctorPolicy policy;
    UmiRepositoryDoctorReport report;
    UmiStatus last_status;
    uint64_t revision;
} UmiRepositoryMaintenanceService;

void umi_repository_maintenance_service_init(UmiRepositoryMaintenanceService *service);
UmiStatus umi_repository_maintenance_service_evaluate(UmiRepositoryMaintenanceService *service);

#ifdef __cplusplus
}
#endif
#endif
