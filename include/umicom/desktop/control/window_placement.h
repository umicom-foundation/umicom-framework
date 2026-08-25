/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/window_placement.h
 *
 * PURPOSE:
 *   Represent toolkit-neutral window geometry and monitor affinity.
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
#ifndef UMICOM_DESKTOP_CONTROL_WINDOW_PLACEMENT_H
#define UMICOM_DESKTOP_CONTROL_WINDOW_PLACEMENT_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_WINDOW_PLACEMENT_API_VERSION 1U

typedef struct UmiDesktopWindowPlacementSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopWindowPlacementSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_window_placement_init(UmiDesktopWindowPlacementSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_window_placement_validate(const UmiDesktopWindowPlacementSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_window_placement_set_subject(UmiDesktopWindowPlacementSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_window_placement_set_detail(UmiDesktopWindowPlacementSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_window_placement_set_state(UmiDesktopWindowPlacementSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_window_placement_set_priority(UmiDesktopWindowPlacementSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_window_placement_set_enabled(UmiDesktopWindowPlacementSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_window_placement_same_identity(const UmiDesktopWindowPlacementSnapshot *left, const UmiDesktopWindowPlacementSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
UmiStatus umi_desktop_window_placement_clamp(UmiDesktopControlRect *geometry, const UmiDesktopControlRect *work_area);

#ifdef __cplusplus
}
#endif
#endif
