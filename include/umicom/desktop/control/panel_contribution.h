/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/panel_contribution.h
 *
 * PURPOSE:
 *   Describe a panel contribution while leaving rendering to toolkit adapters.
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
#ifndef UMICOM_DESKTOP_CONTROL_PANEL_CONTRIBUTION_H
#define UMICOM_DESKTOP_CONTROL_PANEL_CONTRIBUTION_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_PANEL_CONTRIBUTION_API_VERSION 1U

typedef struct UmiDesktopPanelContributionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopPanelContributionSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_panel_contribution_init(UmiDesktopPanelContributionSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_panel_contribution_validate(const UmiDesktopPanelContributionSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_panel_contribution_set_subject(UmiDesktopPanelContributionSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_panel_contribution_set_detail(UmiDesktopPanelContributionSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_panel_contribution_set_state(UmiDesktopPanelContributionSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_panel_contribution_set_priority(UmiDesktopPanelContributionSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_panel_contribution_set_enabled(UmiDesktopPanelContributionSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_panel_contribution_same_identity(const UmiDesktopPanelContributionSnapshot *left, const UmiDesktopPanelContributionSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
