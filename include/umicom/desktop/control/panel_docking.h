/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/panel_docking.h
 *
 * PURPOSE:
 *   Describe docking intent and target location for a panel.
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
#ifndef UMICOM_DESKTOP_CONTROL_PANEL_DOCKING_H
#define UMICOM_DESKTOP_CONTROL_PANEL_DOCKING_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_PANEL_DOCKING_API_VERSION 1U

/**
 * Represent the desktop panel docking snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopPanelDockingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopPanelDockingSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_panel_docking_init(UmiDesktopPanelDockingSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_panel_docking_validate(const UmiDesktopPanelDockingSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_panel_docking_set_subject(UmiDesktopPanelDockingSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_panel_docking_set_detail(UmiDesktopPanelDockingSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_panel_docking_set_state(UmiDesktopPanelDockingSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_panel_docking_set_priority(UmiDesktopPanelDockingSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_panel_docking_set_enabled(UmiDesktopPanelDockingSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_panel_docking_same_identity(const UmiDesktopPanelDockingSnapshot *left, const UmiDesktopPanelDockingSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
