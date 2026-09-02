/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_update_state.h
 *
 * PURPOSE:
 *   Expose application update availability and channel state to any shell.
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
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_UPDATE_STATE_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_UPDATE_STATE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_UPDATE_STATE_API_VERSION 1U

/**
 * Represent the desktop application update state snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDesktopApplicationUpdateStateSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationUpdateStateSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_update_state_init(UmiDesktopApplicationUpdateStateSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_update_state_validate(const UmiDesktopApplicationUpdateStateSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_update_state_set_subject(UmiDesktopApplicationUpdateStateSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_update_state_set_detail(UmiDesktopApplicationUpdateStateSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_update_state_set_state(UmiDesktopApplicationUpdateStateSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_update_state_set_priority(UmiDesktopApplicationUpdateStateSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_update_state_set_enabled(UmiDesktopApplicationUpdateStateSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_update_state_same_identity(const UmiDesktopApplicationUpdateStateSnapshot *left, const UmiDesktopApplicationUpdateStateSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
