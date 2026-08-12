/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/project_bootstrap.c
 *
 * PURPOSE:
 *   Implement filesystem project bootstrap over the reusable Project Workspace
 *   importer and the existing dependency-aware Developer Runtime workflow API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Bootstrap only creates project metadata and queued developer operations. It
 * does not execute external commands. Callers explicitly choose an executor
 * later, preserving the Framework's separation between planning and execution.
 */
#include "umicom/developer/project_bootstrap.h"

#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiStatus set_project_context(
    UmiDeveloperRuntime *runtime,
    const UmiProjectWorkspaceSelectionSnapshot *selection)
{
    UmiDeveloperContextSnapshot context;

    if (runtime == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&context, 0, sizeof(context));
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    copy_text(context.project_id, sizeof(context.project_id), selection->project.id);
    if (selection->has_configuration) {
        copy_text(context.configuration_id, sizeof(context.configuration_id),
                  selection->configuration.id);
    }
    if (selection->has_target) {
        copy_text(context.target_id, sizeof(context.target_id), selection->target.id);
    }
    if (selection->has_launch_profile) {
        copy_text(context.launch_profile_id, sizeof(context.launch_profile_id),
                  selection->launch_profile.id);
    }
    copy_text(context.workspace_directory, sizeof(context.workspace_directory),
              selection->project.root_uri);
    return umi_developer_runtime_set_context(runtime, &context);
}

UmiStatus umi_developer_project_bootstrap(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot)
{
    UmiDeveloperProjectBootstrapSnapshot snapshot;
    UmiDeveloperProjectWorkflowRequest workflow_request;
    UmiStatus status;

    if (runtime == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.struct_size = (uint32_t)sizeof(snapshot);
    snapshot.api_version = UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;

    status = umi_project_workspace_import_directory(
        umi_developer_runtime_projects(runtime),
        &request->project,
        &snapshot.project);
    if (status != UMI_STATUS_OK) return status;

    status = set_project_context(runtime, &snapshot.project.selection);
    if (status != UMI_STATUS_OK) return status;

    if (request->prepare_workflow != 0) {
        memset(&workflow_request, 0, sizeof(workflow_request));
        workflow_request.struct_size = (uint32_t)sizeof(workflow_request);
        workflow_request.api_version = UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
        workflow_request.preset = request->preset;
        workflow_request.workflow_id = request->workflow_id;
        workflow_request.project_id = snapshot.project.project_id;
        workflow_request.configuration_id = snapshot.project.configuration_id;
        workflow_request.target_id = snapshot.project.target_id;
        workflow_request.environment_id = snapshot.project.environment_id;
        workflow_request.timeout_ms = request->timeout_ms;
        workflow_request.max_attempts = request->max_attempts;
        workflow_request.include_configure = request->include_configure;
        if (snapshot.project.has_launch_profile) {
            workflow_request.launch_profile_id = snapshot.project.launch_profile_id;
        }

        status = umi_developer_project_workflow_submit(
            runtime, &workflow_request, &snapshot.workflow);
        if (status != UMI_STATUS_OK) return status;
        snapshot.workflow_prepared = 1;
    }

    status = umi_developer_context_snapshot(
        umi_developer_runtime_context(runtime), &snapshot.context);
    if (status != UMI_STATUS_OK) return status;

    if (out_snapshot != NULL) *out_snapshot = snapshot;
    return UMI_STATUS_OK;
}
