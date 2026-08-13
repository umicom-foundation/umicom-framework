/*-----------------------------------------------------------------------------
 * Umicom Framework common terminal UI view-model helpers.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "view_internal.h"

#include <stdio.h>

UmiStatus umi_terminal_ui_set_string(UmiUiViewModel *view,
                                     const char *key,
                                     const char *value)
{
    UmiUiValue property;
    UmiStatus status = umi_ui_value_set_string(&property, value != NULL ? value : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &property) : status;
}

UmiStatus umi_terminal_ui_set_integer(UmiUiViewModel *view,
                                      const char *key,
                                      int64_t value)
{
    UmiUiValue property;
    UmiStatus status = umi_ui_value_set_integer(&property, value);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &property) : status;
}

UmiStatus umi_terminal_ui_set_boolean(UmiUiViewModel *view,
                                      const char *key,
                                      int value)
{
    UmiUiValue property;
    UmiStatus status = umi_ui_value_set_boolean(&property, value);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &property) : status;
}

UmiStatus umi_terminal_ui_base_view(const char *view_id,
                                    const char *kind,
                                    const char *title,
                                    const char *summary,
                                    UmiUiViewModel **out_view)
{
    UmiStatus status = umi_ui_view_model_create(
        view_id, "umicom.terminal-ui", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
                                                                     "umicom.view-kind", kind);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
                                                                     "title", title);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
                                                                     "summary", summary);
    if (status != UMI_STATUS_OK && out_view != NULL && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_terminal_ui_set_action(UmiUiViewModel *view,
                                     size_t index,
                                     const char *action_id,
                                     const char *label,
                                     const char *tooltip)
{
    UmiUiCommandViewAction action = {0};
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}
