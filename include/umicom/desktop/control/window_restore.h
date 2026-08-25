/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/window_restore.h
 *
 * PURPOSE:
 *   Represent restoration policy for a persisted window.
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
#ifndef UMICOM_DESKTOP_CONTROL_WINDOW_RESTORE_H
#define UMICOM_DESKTOP_CONTROL_WINDOW_RESTORE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_WINDOW_RESTORE_API_VERSION 1U

typedef struct UmiDesktopWindowRestoreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopWindowRestoreSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_window_restore_init(UmiDesktopWindowRestoreSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_window_restore_validate(const UmiDesktopWindowRestoreSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_window_restore_set_subject(UmiDesktopWindowRestoreSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_window_restore_set_detail(UmiDesktopWindowRestoreSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_window_restore_set_state(UmiDesktopWindowRestoreSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_window_restore_set_priority(UmiDesktopWindowRestoreSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_window_restore_set_enabled(UmiDesktopWindowRestoreSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_window_restore_same_identity(const UmiDesktopWindowRestoreSnapshot *left, const UmiDesktopWindowRestoreSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_window_restore_should_restore(bool persisted_visible, bool provider_available, bool session_clean);

#ifdef __cplusplus
}
#endif
#endif
