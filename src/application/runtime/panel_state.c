/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/panel_state.c
 *
 * PURPOSE:
 *   Implement panel state transitions while honouring immutable experience flags.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/panel_state.h"

#include <string.h>

static int valid_visibility(UmiApplicationPanelVisibility value)
{
    return value >= UMI_APPLICATION_PANEL_HIDDEN &&
           value <= UMI_APPLICATION_PANEL_FLOATING;
}

UmiStatus umi_application_panel_state_init(
    const UmiExperiencePanelDefinition *definition,
    UmiApplicationPanelState *out_state)
{
    if (definition == NULL || out_state == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_state, 0, sizeof(*out_state));
    out_state->structure_size = sizeof(*out_state);
    out_state->definition = definition;
    out_state->visibility = UMI_APPLICATION_PANEL_VISIBLE;
    return UMI_STATUS_OK;
}

int umi_application_panel_state_can_float(const UmiApplicationPanelState *state)
{
    return state != NULL && state->definition != NULL &&
        (state->definition->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) != 0U;
}

int umi_application_panel_state_can_auto_hide(const UmiApplicationPanelState *state)
{
    return state != NULL && state->definition != NULL &&
        (state->definition->flags & UMI_EXPERIENCE_PANEL_AUTO_HIDE) != 0U;
}

UmiStatus umi_application_panel_state_set_visibility(
    UmiApplicationPanelState *state,
    UmiApplicationPanelVisibility visibility)
{
    if (state == NULL || !valid_visibility(visibility))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (visibility == UMI_APPLICATION_PANEL_FLOATING &&
        !umi_application_panel_state_can_float(state))
        return UMI_STATUS_PERMISSION_DENIED;
    if (visibility == UMI_APPLICATION_PANEL_AUTO_HIDDEN &&
        !umi_application_panel_state_can_auto_hide(state))
        return UMI_STATUS_PERMISSION_DENIED;
    if (state->visibility != visibility) {
        state->visibility = visibility;
        state->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_panel_state_set_focus(
    UmiApplicationPanelState *state,
    bool focused)
{
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (state->focused != focused) {
        state->focused = focused;
        state->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_panel_state_set_monitor(
    UmiApplicationPanelState *state,
    unsigned monitor_index)
{
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (monitor_index != 0U && state->definition != NULL &&
        (state->definition->flags & UMI_EXPERIENCE_PANEL_MULTI_MONITOR) == 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    if (state->monitor_index != monitor_index) {
        state->monitor_index = monitor_index;
        state->revision += 1U;
    }
    return UMI_STATUS_OK;
}
