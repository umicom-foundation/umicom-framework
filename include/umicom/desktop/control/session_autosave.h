/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/session_autosave.h
 *
 * PURPOSE:
 *   Store crash-safe session autosave cadence and pending state.
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
#ifndef UMICOM_DESKTOP_CONTROL_SESSION_AUTOSAVE_H
#define UMICOM_DESKTOP_CONTROL_SESSION_AUTOSAVE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SESSION_AUTOSAVE_API_VERSION 1U

/**
 * Represent the desktop session autosave snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopSessionAutosaveSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSessionAutosaveSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_session_autosave_init(UmiDesktopSessionAutosaveSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_session_autosave_validate(const UmiDesktopSessionAutosaveSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_session_autosave_set_subject(UmiDesktopSessionAutosaveSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_session_autosave_set_detail(UmiDesktopSessionAutosaveSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_session_autosave_set_state(UmiDesktopSessionAutosaveSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_session_autosave_set_priority(UmiDesktopSessionAutosaveSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_session_autosave_set_enabled(UmiDesktopSessionAutosaveSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_session_autosave_same_identity(const UmiDesktopSessionAutosaveSnapshot *left, const UmiDesktopSessionAutosaveSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_session_autosave_due(uint64_t now_ms, uint64_t last_save_ms, uint64_t interval_ms, bool dirty);

#ifdef __cplusplus
}
#endif
#endif
