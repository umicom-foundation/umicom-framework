/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/maintenance_types.h
 *
 * PURPOSE:
 *   Define repository maintenance health and diagnostic vocabulary.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_TYPES_H
#define INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_TYPES_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named repository maintenance state values accepted by this public contract.
 */
typedef enum UmiRepositoryMaintenanceState {
    UMI_REPOSITORY_MAINTENANCE_CLEAN = 0,
    UMI_REPOSITORY_MAINTENANCE_DIRTY = 1,
    UMI_REPOSITORY_MAINTENANCE_DIVERGED = 2,
    UMI_REPOSITORY_MAINTENANCE_INCOMPLETE = 3,
    UMI_REPOSITORY_MAINTENANCE_ERROR = 4
} UmiRepositoryMaintenanceState;

/**
 * List the named repository doctor severity values accepted by this public contract.
 */
typedef enum UmiRepositoryDoctorSeverity {
    UMI_REPOSITORY_DOCTOR_INFO = 0,
    UMI_REPOSITORY_DOCTOR_WARNING = 1,
    UMI_REPOSITORY_DOCTOR_ERROR = 2
} UmiRepositoryDoctorSeverity;

/**
 * Provide the repository maintenance state text operation used by this module and its
 * client applications.
 */
const char *umi_repository_maintenance_state_text(UmiRepositoryMaintenanceState state);
/**
 * Provide the repository doctor severity text operation used by this module and its client
 * applications.
 */
const char *umi_repository_doctor_severity_text(UmiRepositoryDoctorSeverity severity);

#ifdef __cplusplus
}
#endif
#endif
