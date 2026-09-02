/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/view_helpers.c
 *
 * PURPOSE:
 *   Implement common Studio view-model projection helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/view_helpers.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the studio view set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_string(
    UmiUiViewModel *view,
    const char *key,
    const char *text)
{
    UmiUiValue value;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/*
 * Provide the studio view set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_integer(
    UmiUiViewModel *view,
    const char *key,
    int64_t number)
{
    UmiUiValue value;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/*
 * Provide the studio view set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_boolean(
    UmiUiViewModel *view,
    const char *key,
    int enabled)
{
    UmiUiValue value;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/*
 * Provide the studio view set action operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_action(
    UmiUiViewModel *view,
    size_t index,
    const char *command_id,
    const char *label,
    const char *tooltip,
    int enabled)
{
    UmiUiCommandViewAction action;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || command_id == NULL ||
        label == NULL || tooltip == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&action, 0, sizeof(action));
    (void)snprintf(
        action.action_id,
        sizeof(action.action_id),
        "%s",
        command_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = enabled != 0;

    return umi_ui_command_view_set_action(view, index, &action);
}

/*
 * Provide the studio view create base operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_create_base(
    const char *view_id,
    const char *kind,
    const char *title,
    const char *summary,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL || kind == NULL ||
        title == NULL || summary == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_view = NULL;

    status = umi_ui_view_model_create(
        view_id,
        "umicom.studio-runtime",
        UMI_UI_ROLE_PANE,
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "umicom.view-kind", kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(*out_view, "title", title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(*out_view, "summary", summary);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }

    return status;
}
