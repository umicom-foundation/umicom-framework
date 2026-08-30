/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/workbench_availability.h
 *
 * PURPOSE:
 *   Represent workbench availability and missing capability detail.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_WORKBENCH_AVAILABILITY_H
#define UMICOM_DESKTOP_CONTROL_WORKBENCH_AVAILABILITY_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_WORKBENCH_AVAILABILITY_API_VERSION 1U

typedef struct UmiDesktopWorkbenchAvailabilitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopWorkbenchAvailabilitySnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_workbench_availability_init(UmiDesktopWorkbenchAvailabilitySnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_workbench_availability_validate(const UmiDesktopWorkbenchAvailabilitySnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_workbench_availability_set_subject(UmiDesktopWorkbenchAvailabilitySnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_workbench_availability_set_detail(UmiDesktopWorkbenchAvailabilitySnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_workbench_availability_set_state(UmiDesktopWorkbenchAvailabilitySnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_workbench_availability_set_priority(UmiDesktopWorkbenchAvailabilitySnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_workbench_availability_set_enabled(UmiDesktopWorkbenchAvailabilitySnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_workbench_availability_same_identity(const UmiDesktopWorkbenchAvailabilitySnapshot *left, const UmiDesktopWorkbenchAvailabilitySnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_workbench_availability_selectable(UmiDesktopControlAvailability availability);

#ifdef __cplusplus
}
#endif
#endif
