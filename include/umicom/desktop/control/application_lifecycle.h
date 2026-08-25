/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_lifecycle.h
 *
 * PURPOSE:
 *   Track requested application lifecycle intent and bounded transition state.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_LIFECYCLE_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_LIFECYCLE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_LIFECYCLE_API_VERSION 1U

typedef struct UmiDesktopApplicationLifecycleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationLifecycleSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_lifecycle_init(UmiDesktopApplicationLifecycleSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_lifecycle_validate(const UmiDesktopApplicationLifecycleSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_lifecycle_set_subject(UmiDesktopApplicationLifecycleSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_lifecycle_set_detail(UmiDesktopApplicationLifecycleSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_lifecycle_set_state(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_lifecycle_set_priority(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_lifecycle_set_enabled(UmiDesktopApplicationLifecycleSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_lifecycle_same_identity(const UmiDesktopApplicationLifecycleSnapshot *left, const UmiDesktopApplicationLifecycleSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_application_lifecycle_transition_allowed(UmiDesktopControlLifecycleState from_state, UmiDesktopControlLifecycleState to_state);

#ifdef __cplusplus
}
#endif
#endif
