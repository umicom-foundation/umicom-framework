/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/maintenance_types.c
 *
 * PURPOSE:
 *   Implement repository maintenance state and severity names.
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
#include "umicom/repository/maintenance_types.h"

/*
 * Provide the repository maintenance state text operation used by this module and its
 * client applications.
 */
const char *umi_repository_maintenance_state_text(UmiRepositoryMaintenanceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_REPOSITORY_MAINTENANCE_CLEAN: return "clean";
        case UMI_REPOSITORY_MAINTENANCE_DIRTY: return "dirty";
        case UMI_REPOSITORY_MAINTENANCE_DIVERGED: return "diverged";
        case UMI_REPOSITORY_MAINTENANCE_INCOMPLETE: return "incomplete";
        case UMI_REPOSITORY_MAINTENANCE_ERROR: return "error";
        default: return "unknown";
    }
}

/*
 * Provide the repository doctor severity text operation used by this module and its client
 * applications.
 */
const char *umi_repository_doctor_severity_text(UmiRepositoryDoctorSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_REPOSITORY_DOCTOR_INFO: return "info";
        case UMI_REPOSITORY_DOCTOR_WARNING: return "warning";
        case UMI_REPOSITORY_DOCTOR_ERROR: return "error";
        default: return "unknown";
    }
}
