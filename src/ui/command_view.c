/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/command_view.c
 *
 * PURPOSE:
 *   Implement Framework-owned command-view properties shared by GTK4,
 *   headless tests and future Qt/Wt frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/command_view.h"

#include <stdio.h>
#include <string.h>

/* Provide the make key operation used by this module and its client applications. */
static UmiStatus make_key(size_t index,
                          const char *suffix,
                          char *out_key,
                          size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index >= UMI_UI_COMMAND_VIEW_ACTION_MAX || suffix == NULL ||
        out_key == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_key, capacity, "command-view.action.%02zu.%s",
                       index, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(UmiUiViewModel *view,
                            size_t index,
                            const char *suffix,
                            const char *text)
{
    UmiUiValue value;
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    UmiStatus status = make_key(index, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    }
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/*
 * Provide the ui command view set action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_command_view_set_action(UmiUiViewModel *view,
                                         size_t index,
                                         const UmiUiCommandViewAction *action)
{
    UmiUiValue value;
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || action == NULL || action->action_id[0] == '\0' ||
        action->label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = set_string(view, index, "id", action->action_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = set_string(view, index, "label", action->label);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = set_string(view, index, "tooltip", action->tooltip);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = make_key(index, "enabled", key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_value_set_boolean(&value, action->enabled);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_view_model_set_property(view, key, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_value_set_integer(&value, (int64_t)(index + 1U));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(
            view, UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY, &value);
    }
    return status;
}

/* Provide the get string operation used by this module and its client applications. */
static UmiStatus get_string(const UmiUiViewModel *view,
                            size_t index,
                            const char *suffix,
                            char *out_text,
                            size_t capacity)
{
    UmiUiValue value;
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    size_t length;
    UmiStatus status = make_key(index, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_view_model_get_property(view, key, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || value.kind != UMI_UI_VALUE_STRING) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }
    length = strlen(value.string_value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text, value.string_value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Find ui command view action while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_command_view_action_at(const UmiUiViewModel *view,
                                        size_t index,
                                        UmiUiCommandViewAction *out_action)
{
    UmiUiValue value;
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || out_action == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_action, 0, sizeof(*out_action));
    status = get_string(view, index, "id", out_action->action_id,
                        sizeof(out_action->action_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = get_string(view, index, "label", out_action->label,
                            sizeof(out_action->label));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = get_string(view, index, "tooltip", out_action->tooltip,
                            sizeof(out_action->tooltip));
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = make_key(index, "enabled", key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_view_model_get_property(view, key, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && value.kind != UMI_UI_VALUE_BOOLEAN) {
        status = UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) out_action->enabled = value.boolean_value;
    return status;
}

/*
 * Provide the ui command view property is reserved operation used by this module and its
 * client applications.
 */
int umi_ui_command_view_property_is_reserved(const char *property_key)
{
    return property_key != NULL &&
           strncmp(property_key, "command-view.", 13U) == 0;
}
