/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/view_helpers.c
 *
 * PURPOSE:
 *   Implement common view-property and command-action projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/view_helpers.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_developer_view_set_string(
    UmiUiViewModel *view,
    const char *key,
    const char *text)
{
    UmiUiValue value;
    UmiStatus status;

    if (view == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_value_set_string(
        &value,
        text != NULL ? text : "");

    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

UmiStatus umi_ai_developer_view_set_integer(
    UmiUiViewModel *view,
    const char *key,
    int64_t value_number)
{
    UmiUiValue value;
    UmiStatus status;

    if (view == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_value_set_integer(&value, value_number);

    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

UmiStatus umi_ai_developer_view_set_boolean(
    UmiUiViewModel *view,
    const char *key,
    int enabled)
{
    UmiUiValue value;
    UmiStatus status;

    if (view == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_value_set_boolean(&value, enabled);

    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

UmiStatus umi_ai_developer_view_set_action(
    UmiUiViewModel *view,
    size_t index,
    const char *action_id,
    const char *label,
    const char *tooltip,
    int enabled)
{
    UmiUiCommandViewAction action;

    if (view == NULL || action_id == NULL ||
        label == NULL || tooltip == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&action, 0, sizeof(action));
    (void)snprintf(
        action.action_id,
        sizeof(action.action_id),
        "%s",
        action_id);
    (void)snprintf(
        action.label,
        sizeof(action.label),
        "%s",
        label);
    (void)snprintf(
        action.tooltip,
        sizeof(action.tooltip),
        "%s",
        tooltip);
    action.enabled = enabled != 0;

    return umi_ui_command_view_set_action(view, index, &action);
}

UmiStatus umi_ai_developer_view_create_base(
    const char *view_id,
    const char *kind,
    const char *title,
    const char *summary,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

    if (view_id == NULL || kind == NULL ||
        title == NULL || summary == NULL ||
        out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_view = NULL;

    status = umi_ui_view_model_create(
        view_id,
        "umicom.ai-developer-experience",
        UMI_UI_ROLE_PANE,
        out_view);

    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_view_set_string(
            *out_view, "umicom.view-kind", kind);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_view_set_string(
            *out_view, "title", title);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_view_set_string(
            *out_view, "summary", summary);
    }

    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }

    return status;
}
