/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/workspace_runtime.c
 *
 * PURPOSE:
 *   Coordinate canonical session, command and context state while retaining Framework ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/workspace_runtime.h"

#include <string.h>

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

UmiStatus umi_application_workspace_runtime_select_layout(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *layout_id)
{
    UmiStatus result;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_select_layout(&runtime->session, layout_id);
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
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_PANEL_ACTIVATE, panel_id, result);
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
    (void)umi_application_operation_log_record(&runtime->operations,
        UMI_APPLICATION_OPERATION_CONTEXT_CHANGE, group_id, result);
    return result;
}
