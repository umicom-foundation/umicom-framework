/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/views.c
 *
 * PURPOSE:
 *   Implement the views behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop semantic views
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/views.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set integer operation used by this module and its client applications. */
static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set boolean operation used by this module and its client applications. */
static UmiStatus set_boolean(UmiUiViewModel *view, const char *key, bool flag)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, flag ? 1 : 0);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the create base operation used by this module and its client applications. */
static UmiStatus create_base(const char *view_id, const char *view_type,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status = umi_ui_view_model_create(
        view_id, view_type, UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && out_view != NULL && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/* Provide the add actions operation used by this module and its client applications. */
static UmiStatus add_actions(UmiUiViewModel *view,
                             UmiDesktopLayoutDesigner *designer)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_desktop_shell_action_count(); ++index) {
        UmiDesktopShellAction source;
        UmiUiCommandViewAction target = {0};
        UmiStatus status = umi_desktop_shell_action_at(
            designer, index, &source);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(target.action_id, sizeof(target.action_id), "%s",
                       source.action_id);
        (void)snprintf(target.label, sizeof(target.label), "%s", source.label);
        (void)snprintf(target.tooltip, sizeof(target.tooltip), "%s",
                       source.tooltip);
        target.enabled = source.enabled ? 1 : 0;
        status = umi_ui_command_view_set_action(view, index, &target);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise desktop overview view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_desktop_overview_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view)
{
    UmiDesktopShellSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_shell_model_snapshot(model, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = create_base(
        view_id, "umicom.desktop.overview", "Umicom Desktop",
        "Framework Master Controller view of layouts, application windows, context links and monitors.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "active-layout-id",
                            snapshot.active_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "active-layout-name",
                            snapshot.active_layout_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "layout-count",
                             (int64_t)snapshot.tab_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "window-count",
                             (int64_t)snapshot.window_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "monitor-count",
                             (int64_t)snapshot.monitor_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "context-group-count",
                             (int64_t)snapshot.context_group_count);
    return status;
}

/*
 * Initialise desktop layout designer view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_layout_designer_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view)
{
    UmiDesktopShellSnapshot shell;
    UmiDesktopLayoutDesignerSnapshot designer;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_shell_model_snapshot(model, &shell);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_designer_snapshot(
            umi_desktop_shell_model_designer(model), &designer);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = create_base(
        view_id, "umicom.desktop.layout-designer", "Visual Layout Designer",
        "Snap, dock, float and arrange independent Umicom application windows on a shared multi-monitor canvas.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "active-layout", shell.active_layout_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "designer-active", designer.active);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "layout-locked",
                             shell.active_layout_locked);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "selected-window",
                            designer.selected_window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "undo-count",
                             (int64_t)designer.undo_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "redo-count",
                             (int64_t)designer.redo_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = add_actions(*out_view,
                             umi_desktop_shell_model_designer(model));
    return status;
}

/*
 * Initialise desktop monitor manager view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_monitor_manager_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view)
{
    UmiDesktopShellSnapshot snapshot;
    UmiDesktopSnapshot desktop;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_shell_model_snapshot(model, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_runtime_snapshot(
            umi_desktop_shell_model_runtime(model), &desktop);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = create_base(
        view_id, "umicom.desktop.monitor-manager", "Monitor Manager",
        "Monitor-aware presentation for detachable, floating and distributed Umicom application windows.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "monitor-count",
                             (int64_t)snapshot.monitor_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "primary-monitor-id",
                            desktop.primary_monitor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "visible-window-count",
                             (int64_t)snapshot.window_count);
    return status;
}
