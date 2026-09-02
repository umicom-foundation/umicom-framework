/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/panel_activation.h
 *
 * PURPOSE:
 *   Track active panel state for command and context routing.
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
#ifndef UMICOM_DESKTOP_CONTROL_PANEL_ACTIVATION_H
#define UMICOM_DESKTOP_CONTROL_PANEL_ACTIVATION_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_PANEL_ACTIVATION_API_VERSION 1U

/**
 * Represent the desktop panel activation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopPanelActivationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopPanelActivationSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_panel_activation_init(UmiDesktopPanelActivationSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_panel_activation_validate(const UmiDesktopPanelActivationSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_panel_activation_set_subject(UmiDesktopPanelActivationSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_panel_activation_set_detail(UmiDesktopPanelActivationSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_panel_activation_set_state(UmiDesktopPanelActivationSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_panel_activation_set_priority(UmiDesktopPanelActivationSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_panel_activation_set_enabled(UmiDesktopPanelActivationSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_panel_activation_same_identity(const UmiDesktopPanelActivationSnapshot *left, const UmiDesktopPanelActivationSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
