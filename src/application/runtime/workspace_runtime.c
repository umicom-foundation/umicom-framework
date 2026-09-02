/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/workspace_runtime.c
 *
 * PURPOSE:
 *   Coordinate canonical session, command and context state while retaining Framework ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/workspace_runtime.h"

#include <string.h>

#include "umicom/application/runtime/layout_session.h"
#include "umicom/application/runtime/panel_state.h"

/* Project active application panels into the workbench pane model already owned by UI. */
static UmiStatus sync_panels(UmiApplicationWorkspaceRuntime *runtime)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    UmiUiPaneModel *panes = umi_ui_workbench_panes(runtime->workbench);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.experience->panel_count &&
         status == UMI_STATUS_OK; ++index) {
        UmiApplicationPanelState state;
        const UmiExperiencePanelDefinition *definition =
            &runtime->session.experience->panels[index];
        status = umi_application_panel_state_init(definition, &state);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_application_session_panel_active(&runtime->session, definition->panel_id))
            status = umi_application_panel_state_set_visibility(
                &state, UMI_APPLICATION_PANEL_HIDDEN);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = umi_application_panel_state_apply_to_panes(
                &state, panes, (int32_t)index);
    }
    return status;
}

/* Seed missing experience layouts into the existing workspace-profile catalogue. */
static UmiStatus sync_layouts(UmiApplicationWorkspaceRuntime *runtime)
{
    UmiApplicationLayoutSession layout_session;
    UmiStatus status = umi_application_layout_session_init(
        runtime->session.experience, &layout_session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Mirror the compatibility session state into the canonical workspace
     * profile model rather than maintaining a second frontend layout system. */
    layout_session.selected_layout = runtime->session.layout;
    layout_session.locked = runtime->session.layout_locked;
    return umi_application_layout_session_apply_to_profiles(
        &layout_session, umi_ui_workbench_workspace_profiles(runtime->workbench));
}

/*
 * Initialise application workspace runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_workspace_runtime_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_runtime, 0, sizeof(*out_runtime));
    out_runtime->structure_size = sizeof(*out_runtime);
    result = umi_application_session_init(experience, &out_runtime->session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    umi_application_context_binding_store_init(&out_runtime->contexts);
    result = umi_application_command_surface_build(experience, &out_runtime->commands);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    umi_application_operation_log_init(&out_runtime->operations);
    return umi_application_operation_log_record(&out_runtime->operations,
        UMI_APPLICATION_OPERATION_SESSION_START, experience->application_id, UMI_STATUS_OK);
}

/* Synchronise compatibility runtime state into canonical workbench models after binding. */
UmiStatus umi_application_workspace_runtime_sync_workbench(
    UmiApplicationWorkspaceRuntime *runtime)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || runtime->workbench == NULL)
        return UMI_STATUS_INVALID_STATE;

    /* Synchronise into the pre-existing workbench models in one direction.
     * Frontends continue to persist and render those canonical UI models. */
    result = sync_layouts(runtime);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->session.layout != NULL)
        result = umi_ui_workbench_activate_workspace_profile(
            runtime->workbench, runtime->session.layout->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_OK) result = sync_panels(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_OK)
        result = umi_application_context_binding_apply_to_ui(
            &runtime->contexts, umi_ui_workbench_context(runtime->workbench));
    return result;
}

/* Attach an existing workbench and immediately project the current runtime state into it. */
UmiStatus umi_application_workspace_runtime_bind_workbench(
    UmiApplicationWorkspaceRuntime *runtime,
    UmiUiWorkbench *workbench)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime->workbench = workbench;
    return umi_application_workspace_runtime_sync_workbench(runtime);
}

/* Detach presentation state without destroying either the runtime or the workbench. */
void umi_application_workspace_runtime_unbind_workbench(
    UmiApplicationWorkspaceRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) runtime->workbench = NULL;
}

/*
 * Provide the application workspace runtime select layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_workspace_runtime_select_layout(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *layout_id)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_select_layout(&runtime->session, layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_application_workspace_runtime_sync_workbench(runtime);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_LAYOUT_CHANGE, layout_id, result);
    return result;
}

/*
 * Provide the application workspace runtime activate panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_workspace_runtime_activate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_activate_panel(&runtime->session, panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_ALREADY_EXISTS) result = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = sync_panels(runtime);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_PANEL_ACTIVATE, panel_id, result);
    return result;
}

/* Deactivate one canonical experience panel and update the bound pane model when present. */
UmiStatus umi_application_workspace_runtime_deactivate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_deactivate_panel(&runtime->session, panel_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = sync_panels(runtime);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_PANEL_DEACTIVATE, panel_id, result);
    return result;
}

/* Reuse the existing application-session lock and mirror it to the selected UI profile. */
UmiStatus umi_application_workspace_runtime_set_layout_locked(
    UmiApplicationWorkspaceRuntime *runtime,
    bool locked)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_set_layout_locked(&runtime->session, locked);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_application_workspace_runtime_sync_workbench(runtime);
    return result;
}

/*
 * Provide the application workspace runtime set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_workspace_runtime_set_context(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *group_id,
    const char *value)
{
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_context_binding_set(&runtime->contexts, group_id, value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_ui_context_set_string(
            umi_ui_workbench_context(runtime->workbench), group_id, value);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_CONTEXT_CHANGE, group_id, result);
    return result;
}
