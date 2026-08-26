/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/panel_state.h
 *
 * PURPOSE:
 *   Track toolkit-neutral visibility, focus and monitor placement for one experience panel.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_PANEL_STATE_H
#define UMICOM_APPLICATION_RUNTIME_PANEL_STATE_H

#include "umicom/application/runtime/types.h"
#include "umicom/ui/pane.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPanelState {
    uint32_t structure_size;
    const UmiExperiencePanelDefinition *definition;
    UmiApplicationPanelVisibility visibility;
    bool focused;
    unsigned monitor_index;
    uint64_t revision;
} UmiApplicationPanelState;

UmiStatus umi_application_panel_state_init(
    const UmiExperiencePanelDefinition *definition,
    UmiApplicationPanelState *out_state);
UmiStatus umi_application_panel_state_set_visibility(
    UmiApplicationPanelState *state,
    UmiApplicationPanelVisibility visibility);
UmiStatus umi_application_panel_state_set_focus(
    UmiApplicationPanelState *state,
    bool focused);
UmiStatus umi_application_panel_state_set_monitor(
    UmiApplicationPanelState *state,
    unsigned monitor_index);
int umi_application_panel_state_can_float(
    const UmiApplicationPanelState *state);
int umi_application_panel_state_can_auto_hide(
    const UmiApplicationPanelState *state);
/* Project application metadata into the existing canonical pane model.
 * The application runtime does not create or own a second pane catalogue. */
UmiStatus umi_application_panel_state_apply_to_panes(
    const UmiApplicationPanelState *state,
    UmiUiPaneModel *panes,
    int32_t order);

#ifdef __cplusplus
}
#endif

#endif
