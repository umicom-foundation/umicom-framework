/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/panel_state.h
 *
 * PURPOSE:
 *   Track toolkit-neutral visibility, focus and monitor placement for one experience panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_PANEL_STATE_H
#define UMICOM_APPLICATION_RUNTIME_PANEL_STATE_H

#include "umicom/application/runtime/types.h"
#include "umicom/ui/pane.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application panel state data shared with callers of this public contract.
 */
typedef struct UmiApplicationPanelState {
    uint32_t structure_size;
    const UmiExperiencePanelDefinition *definition;
    UmiApplicationPanelVisibility visibility;
    bool focused;
    unsigned monitor_index;
    uint64_t revision;
} UmiApplicationPanelState;

/**
 * Initialise application panel state from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_panel_state_init(
    const UmiExperiencePanelDefinition *definition,
    UmiApplicationPanelState *out_state);
/**
 * Provide the application panel state set visibility operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_panel_state_set_visibility(
    UmiApplicationPanelState *state,
    UmiApplicationPanelVisibility visibility);
/**
 * Provide the application panel state set focus operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_panel_state_set_focus(
    UmiApplicationPanelState *state,
    bool focused);
/**
 * Provide the application panel state set monitor operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_panel_state_set_monitor(
    UmiApplicationPanelState *state,
    unsigned monitor_index);
/**
 * Provide the application panel state can float operation used by this module and its
 * client applications.
 */
int umi_application_panel_state_can_float(
    const UmiApplicationPanelState *state);
/**
 * Provide the application panel state can auto hide operation used by this module and its
 * client applications.
 */
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
