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

    for (index = 0U; index < runtime->session.experience->panel_count &&
         status == UMI_STATUS_OK; ++index) {
        UmiApplicationPanelState state;
        const UmiExperiencePanelDefinition *definition =
            &runtime->session.experience->panels[index];
        status = umi_application_panel_state_init(definition, &state);
        if (status != UMI_STATUS_OK) break;
        if (!umi_application_session_panel_active(&runtime->session, definition->panel_id))
            status = umi_application_panel_state_set_visibility(
                &state, UMI_APPLICATION_PANEL_HIDDEN);
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
    if (status != UMI_STATUS_OK) return status;

    /* Mirror the compatibility session state into the canonical workspace
     * profile model rather than maintaining a second frontend layout system. */
    layout_session.selected_layout = runtime->session.layout;
    layout_session.locked = runtime->session.layout_locked;
    return umi_application_layout_session_apply_to_profiles(
        &layout_session, umi_ui_workbench_workspace_profiles(runtime->workbench));
}

UmiStatus umi_application_workspace_runtime_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime)
{
    UmiStatus result;
    if (experience == NULL || out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_runtime, 0, sizeof(*out_runtime));
    out_runtime->structure_size = sizeof(*out_runtime);
    result = umi_application_session_init(experience, &out_runtime->session);
    if (result != UMI_STATUS_OK) return result;
    umi_application_context_binding_store_init(&out_runtime->contexts);
    result = umi_application_command_surface_build(experience, &out_runtime->commands);
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
    if (runtime == NULL || runtime->workbench == NULL)
        return UMI_STATUS_INVALID_STATE;

    /* Synchronise into the pre-existing workbench models in one direction.
     * Frontends continue to persist and render those canonical UI models. */
    result = sync_layouts(runtime);
    if (result == UMI_STATUS_OK && runtime->session.layout != NULL)
        result = umi_ui_workbench_activate_workspace_profile(
            runtime->workbench, runtime->session.layout->layout_id);
    if (result == UMI_STATUS_OK) result = sync_panels(runtime);
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
    if (runtime == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime->workbench = workbench;
    return umi_application_workspace_runtime_sync_workbench(runtime);
}

/* Detach presentation state without destroying either the runtime or the workbench. */
void umi_application_workspace_runtime_unbind_workbench(
    UmiApplicationWorkspaceRuntime *runtime)
{
    if (runtime != NULL) runtime->workbench = NULL;
}

UmiStatus umi_application_workspace_runtime_select_layout(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *layout_id)
{
    UmiStatus result;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_select_layout(&runtime->session, layout_id);
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_application_workspace_runtime_sync_workbench(runtime);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_LAYOUT_CHANGE, layout_id, result);
    return result;
}

UmiStatus umi_application_workspace_runtime_activate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id)
{
    UmiStatus result;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_activate_panel(&runtime->session, panel_id);
    if (result == UMI_STATUS_ALREADY_EXISTS) result = UMI_STATUS_OK;
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
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_deactivate_panel(&runtime->session, panel_id);
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
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_set_layout_locked(&runtime->session, locked);
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_application_workspace_runtime_sync_workbench(runtime);
    return result;
}

UmiStatus umi_application_workspace_runtime_set_context(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *group_id,
    const char *value)
{
    UmiStatus result;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_context_binding_set(&runtime->contexts, group_id, value);
    if (result == UMI_STATUS_OK && runtime->workbench != NULL)
        result = umi_ui_context_set_string(
            umi_ui_workbench_context(runtime->workbench), group_id, value);
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_CONTEXT_CHANGE, group_id, result);
    return result;
}
