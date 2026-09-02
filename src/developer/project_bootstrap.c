/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/project_bootstrap.c
 *
 * PURPOSE:
 *   Implement filesystem project bootstrap over the reusable Project Workspace
 *   importer and the existing dependency-aware Developer Runtime workflow API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Bootstrap only creates project metadata and queued developer operations. It
 * does not execute external commands. Callers explicitly choose an executor
 * later, preserving the Framework's separation between planning and execution.
 */
#include "umicom/developer/project_bootstrap.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the set project context operation used by this module and its client
 * applications.
 */
static UmiStatus set_project_context(
    UmiDeveloperRuntime *runtime,
    const UmiProjectWorkspaceSelectionSnapshot *selection)
{
    UmiDeveloperContextSnapshot context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&context, 0, sizeof(context));
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    copy_text(context.project_id, sizeof(context.project_id), selection->project.id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->has_configuration) {
        copy_text(context.configuration_id, sizeof(context.configuration_id),
                  selection->configuration.id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->has_target) {
        copy_text(context.target_id, sizeof(context.target_id), selection->target.id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->has_launch_profile) {
        copy_text(context.launch_profile_id, sizeof(context.launch_profile_id),
                  selection->launch_profile.id);
    }
    copy_text(context.workspace_directory, sizeof(context.workspace_directory),
              selection->project.root_uri);
    return umi_developer_runtime_set_context(runtime, &context);
}

/*
 * Provide the developer project bootstrap operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_bootstrap(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot)
{
    UmiDeveloperProjectWorkflowRequest workflow_request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || request == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

#define snapshot (*out_snapshot)
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.struct_size = (uint32_t)sizeof(snapshot);
    snapshot.api_version = UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;

    status = umi_project_workspace_import_directory(
        umi_developer_runtime_projects(runtime),
        &request->project,
        &snapshot.project);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = set_project_context(runtime, &snapshot.project.selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (snapshot.project.has_launch_profile) {
            workflow_request.launch_profile_id = snapshot.project.launch_profile_id;
        }

        status = umi_developer_project_workflow_submit(
            runtime, &workflow_request, &snapshot.workflow);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        snapshot.workflow_prepared = 1;
    }

    status = umi_developer_context_snapshot(
        umi_developer_runtime_context(runtime), &snapshot.context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

#undef snapshot
    return UMI_STATUS_OK;
}
