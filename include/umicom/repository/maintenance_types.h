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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_TYPES_H
#define INCLUDE_UMICOM_REPOSITORY_MAINTENANCE_TYPES_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiRepositoryMaintenanceState {
    UMI_REPOSITORY_MAINTENANCE_CLEAN = 0,
    UMI_REPOSITORY_MAINTENANCE_DIRTY = 1,
    UMI_REPOSITORY_MAINTENANCE_DIVERGED = 2,
    UMI_REPOSITORY_MAINTENANCE_INCOMPLETE = 3,
    UMI_REPOSITORY_MAINTENANCE_ERROR = 4
} UmiRepositoryMaintenanceState;

typedef enum UmiRepositoryDoctorSeverity {
    UMI_REPOSITORY_DOCTOR_INFO = 0,
    UMI_REPOSITORY_DOCTOR_WARNING = 1,
    UMI_REPOSITORY_DOCTOR_ERROR = 2
} UmiRepositoryDoctorSeverity;

const char *umi_repository_maintenance_state_text(UmiRepositoryMaintenanceState state);
const char *umi_repository_doctor_severity_text(UmiRepositoryDoctorSeverity severity);

#ifdef __cplusplus
}
#endif
#endif
