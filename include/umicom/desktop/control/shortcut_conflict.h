/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/shortcut_conflict.h
 *
 * PURPOSE:
 *   Represent shortcut conflict evidence and resolution priority.
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
#ifndef UMICOM_DESKTOP_CONTROL_SHORTCUT_CONFLICT_H
#define UMICOM_DESKTOP_CONTROL_SHORTCUT_CONFLICT_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SHORTCUT_CONFLICT_API_VERSION 1U

typedef struct UmiDesktopShortcutConflictSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopShortcutConflictSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_shortcut_conflict_init(UmiDesktopShortcutConflictSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_shortcut_conflict_validate(const UmiDesktopShortcutConflictSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_shortcut_conflict_set_subject(UmiDesktopShortcutConflictSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_shortcut_conflict_set_detail(UmiDesktopShortcutConflictSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_shortcut_conflict_set_state(UmiDesktopShortcutConflictSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_shortcut_conflict_set_priority(UmiDesktopShortcutConflictSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_shortcut_conflict_set_enabled(UmiDesktopShortcutConflictSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_shortcut_conflict_same_identity(const UmiDesktopShortcutConflictSnapshot *left, const UmiDesktopShortcutConflictSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_shortcut_conflict_conflicts(const char *left_chord, const char *right_chord, const char *left_context, const char *right_context);

#ifdef __cplusplus
}
#endif
#endif
