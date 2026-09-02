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

/**
 * Represent the repository maintenance service data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryMaintenanceService {
    UmiRepositoryStatusSummary summary;
    UmiRepositoryDoctorPolicy policy;
    UmiRepositoryDoctorReport report;
    UmiStatus last_status;
    uint64_t revision;
} UmiRepositoryMaintenanceService;

/**
 * Initialise repository maintenance service from caller-provided values so later
 * operations receive a known state.
 */
void umi_repository_maintenance_service_init(UmiRepositoryMaintenanceService *service);
/**
 * Provide the repository maintenance service evaluate operation used by this module and
 * its client applications.
 */
UmiStatus umi_repository_maintenance_service_evaluate(UmiRepositoryMaintenanceService *service);

#ifdef __cplusplus
}
#endif
#endif
