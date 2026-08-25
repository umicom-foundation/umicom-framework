/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/session_snapshot.h
 *
 * PURPOSE:
 *   Describe a Desk session snapshot that references layouts, windows and active applications.
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
#ifndef UMICOM_DESKTOP_CONTROL_SESSION_SNAPSHOT_H
#define UMICOM_DESKTOP_CONTROL_SESSION_SNAPSHOT_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SESSION_SNAPSHOT_API_VERSION 1U

typedef struct UmiDesktopSessionSnapshotSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSessionSnapshotSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_session_snapshot_init(UmiDesktopSessionSnapshotSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_session_snapshot_validate(const UmiDesktopSessionSnapshotSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_session_snapshot_set_subject(UmiDesktopSessionSnapshotSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_session_snapshot_set_detail(UmiDesktopSessionSnapshotSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_session_snapshot_set_state(UmiDesktopSessionSnapshotSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_session_snapshot_set_priority(UmiDesktopSessionSnapshotSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_session_snapshot_set_enabled(UmiDesktopSessionSnapshotSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_session_snapshot_same_identity(const UmiDesktopSessionSnapshotSnapshot *left, const UmiDesktopSessionSnapshotSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
