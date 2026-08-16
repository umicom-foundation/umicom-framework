/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/workflow.c
 *
 * PURPOSE:
 *   Implement task/launch workflow composition over the developer runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/workflow.h"
#include "umicom/developer/project_workflow.h"
#include "umicom/developer/project_bootstrap.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus make_operation_id(
    const char *workflow_id,
    const char *kind,
    size_t index,
    char *out_id,
    size_t capacity)
{
    int written = snprintf(out_id, capacity, "%s.%s.%zu", workflow_id, kind, index + 1U);
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workflow_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperWorkflowRequest *request,
    UmiDeveloperWorkflowSnapshot *out_workflow)
{
    UmiDeveloperWorkflowSnapshot workflow;
    UmiDeveloperTaskPlanRequest task_request;
    UmiDeveloperTaskPlanSnapshot task_plan;
    UmiDeveloperLaunchPlanRequest launch_request;
    UmiDeveloperLaunchPlanSnapshot launch_plan;
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char previous_id[UMI_DEVELOPER_ID_CAPACITY];
    size_t index;
    UmiStatus status;

    if (runtime == NULL || request == NULL || request->workflow_id == NULL ||
        request->workflow_id[0] == '\0' ||
        request->task_count > UMI_DEVELOPER_WORKFLOW_MAX_TASKS ||
        (request->task_count > 0U && request->task_ids == NULL) ||
        request->debug_mode < -1 || request->debug_mode > 1) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&workflow, 0, sizeof(workflow));
    workflow.struct_size = (uint32_t)sizeof(workflow);
    workflow.api_version = UMI_DEVELOPER_WORKFLOW_API_VERSION;
    status = copy_text(workflow.workflow_id, sizeof(workflow.workflow_id), request->workflow_id);
    if (status != UMI_STATUS_OK) return status;
    previous_id[0] = '\0';

    for (index = 0U; index < request->task_count; ++index) {
        if (request->task_ids[index] == NULL || request->task_ids[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = make_operation_id(request->workflow_id, "task", index,
                                   operation_id, sizeof(operation_id));
        if (status != UMI_STATUS_OK) return status;
        memset(&task_request, 0, sizeof(task_request));
        task_request.struct_size = (uint32_t)sizeof(task_request);
        task_request.api_version = UMI_DEVELOPER_TASK_PLAN_API_VERSION;
        task_request.task_id = request->task_ids[index];
        task_request.operation_id = operation_id;
        task_request.configuration_id = request->configuration_id;
        task_request.target_id = request->target_id;
        task_request.timeout_ms = request->timeout_ms;
        task_request.max_attempts = request->max_attempts;
        status = umi_developer_task_plan_submit(runtime, &task_request, &task_plan);
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(workflow.operation_ids[workflow.operation_count],
                        sizeof(workflow.operation_ids[workflow.operation_count]),
                        task_plan.operation_id);
        workflow.operation_count += 1U;
        if (previous_id[0] != '\0') {
            status = umi_developer_runtime_add_dependency(
                runtime, task_plan.operation_id, previous_id);
            if (status != UMI_STATUS_OK) return status;
            workflow.dependency_count += 1U;
        }
        (void)copy_text(previous_id, sizeof(previous_id), task_plan.operation_id);
    }

    if (request->launch_profile_id != NULL && request->launch_profile_id[0] != '\0') {
        status = make_operation_id(request->workflow_id, "launch", 0U,
                                   operation_id, sizeof(operation_id));
        if (status != UMI_STATUS_OK) return status;
        memset(&launch_request, 0, sizeof(launch_request));
        launch_request.struct_size = (uint32_t)sizeof(launch_request);
        launch_request.api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
        launch_request.launch_profile_id = request->launch_profile_id;
        launch_request.operation_id = operation_id;
        launch_request.configuration_id = request->configuration_id;
        launch_request.target_id = request->target_id;
        launch_request.timeout_ms = request->timeout_ms;
        launch_request.max_attempts = request->max_attempts;
        launch_request.debug_mode = request->debug_mode;
        status = umi_developer_launch_plan_submit(runtime, &launch_request, &launch_plan);
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(workflow.operation_ids[workflow.operation_count],
                        sizeof(workflow.operation_ids[workflow.operation_count]),
                        launch_plan.operation_id);
        workflow.operation_count += 1U;
        if (previous_id[0] != '\0') {
            status = umi_developer_runtime_add_dependency(
                runtime, launch_plan.operation_id, previous_id);
            if (status != UMI_STATUS_OK) return status;
            workflow.dependency_count += 1U;
        }
    }

    if (workflow.operation_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (out_workflow != NULL) *out_workflow = workflow;
    return UMI_STATUS_OK;
}

/* ------------------------------------------------------------------------- */
/* High-level project workflow resolver.                                      */
/* ------------------------------------------------------------------------- */

static int same_task_id(const char *left, const char *right)
{
    return left != NULL && right != NULL && strcmp(left,right)==0;
}

static UmiStatus append_task(
    UmiDeveloperProjectWorkflowSnapshot *snapshot,
    const UmiProjectTaskSnapshot *task)
{
    size_t index;
    if (snapshot==NULL || task==NULL || task->id[0]=='\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index=0U;index<snapshot->task_count;++index) {
        if (same_task_id(snapshot->task_ids[index],task->id))
            return UMI_STATUS_OK;
    }
    if (snapshot->task_count>=3U) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (copy_text(snapshot->task_ids[snapshot->task_count],
                  sizeof(snapshot->task_ids[snapshot->task_count]),
                  task->id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    snapshot->task_count+=1U;
    return UMI_STATUS_OK;
}

static const char *preset_text(UmiDeveloperProjectWorkflowPreset preset)
{
    switch (preset) {
        case UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD: return "build";
        case UMI_DEVELOPER_PROJECT_WORKFLOW_TEST: return "test";
        case UMI_DEVELOPER_PROJECT_WORKFLOW_RUN: return "run";
        case UMI_DEVELOPER_PROJECT_WORKFLOW_DEBUG: return "debug";
        case UMI_DEVELOPER_PROJECT_WORKFLOW_FULL: return "full";
        default: return "unknown";
    }
}

static UmiStatus build_default_workflow_id(
    const UmiDeveloperProjectWorkflowRequest *request,
    const UmiProjectWorkspaceSelectionSnapshot *selection,
    char *out_id,
    size_t capacity)
{
    int written;
    if (request->workflow_id!=NULL && request->workflow_id[0]!='\0')
        return copy_text(out_id,capacity,request->workflow_id);
    written=snprintf(out_id,capacity,"project.%s.%s",
                     selection->project.id,preset_text(request->preset));
    if (written<0 || (size_t)written>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

static UmiStatus set_project_context(
    UmiDeveloperRuntime *runtime,
    const UmiProjectWorkspaceSelectionSnapshot *selection)
{
    UmiDeveloperContextSnapshot context;
    memset(&context,0,sizeof(context));
    context.struct_size=(uint32_t)sizeof(context);
    context.api_version=UMI_DEVELOPER_CONTEXT_API_VERSION;
    (void)copy_text(context.project_id,sizeof(context.project_id),selection->project.id);
    if (selection->has_configuration)
        (void)copy_text(context.configuration_id,sizeof(context.configuration_id),
                        selection->configuration.id);
    if (selection->has_target)
        (void)copy_text(context.target_id,sizeof(context.target_id),selection->target.id);
    if (selection->has_launch_profile)
        (void)copy_text(context.launch_profile_id,sizeof(context.launch_profile_id),
                        selection->launch_profile.id);
    (void)copy_text(context.workspace_directory,sizeof(context.workspace_directory),
                    selection->project.root_uri);
    return umi_developer_runtime_set_context(runtime,&context);
}

UmiStatus umi_developer_project_workflow_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow)
{
    UmiDeveloperProjectWorkflowSnapshot snapshot;
    UmiProjectWorkspaceSelectionRequest selection_request;
    UmiProjectTaskSnapshot task;
    UmiDeveloperWorkflowRequest workflow_request;
    const char *task_ids[3];
    char workflow_id[UMI_DEVELOPER_ID_CAPACITY];
    UmiStatus status;
    size_t index;
    int needs_test;
    int needs_launch;

    if (runtime==NULL || request==NULL ||
        request->preset<UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD ||
        request->preset>UMI_DEVELOPER_PROJECT_WORKFLOW_FULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&snapshot,0,sizeof(snapshot));
    snapshot.struct_size=(uint32_t)sizeof(snapshot);
    snapshot.api_version=UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    snapshot.preset=request->preset;

    memset(&selection_request,0,sizeof(selection_request));
    selection_request.struct_size=(uint32_t)sizeof(selection_request);
    selection_request.api_version=UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    selection_request.project_id=request->project_id;
    selection_request.configuration_id=request->configuration_id;
    selection_request.target_id=request->target_id;
    selection_request.task_id=request->task_id;
    selection_request.launch_profile_id=request->launch_profile_id;
    selection_request.environment_id=request->environment_id;

    status=umi_project_workspace_resolve_selection(
        umi_developer_runtime_projects(runtime),&selection_request,&snapshot.selection);
    if (status!=UMI_STATUS_OK) return status;

    status=umi_project_workspace_validate_project(
        umi_developer_runtime_projects(runtime),
        snapshot.selection.project.id,&snapshot.validation);
    if (status!=UMI_STATUS_OK) return status;
    if (!snapshot.validation.valid) return UMI_STATUS_INVALID_STATE;

    if (request->include_configure!=0 &&
        umi_project_workspace_find_task_by_group(
            umi_developer_runtime_projects(runtime),
            snapshot.selection.project.id,"configure",&task)==UMI_STATUS_OK) {
        status=append_task(&snapshot,&task);
        if (status!=UMI_STATUS_OK) return status;
    }

    if (umi_project_workspace_find_task_by_group(
            umi_developer_runtime_projects(runtime),
            snapshot.selection.project.id,"build",&task)==UMI_STATUS_OK) {
        status=append_task(&snapshot,&task);
        if (status!=UMI_STATUS_OK) return status;
    } else if (snapshot.selection.has_task) {
        status=append_task(&snapshot,&snapshot.selection.task);
        if (status!=UMI_STATUS_OK) return status;
    }

    needs_test=(request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_TEST ||
                request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_FULL);
    if (needs_test) {
        status=umi_project_workspace_find_task_by_group(
            umi_developer_runtime_projects(runtime),
            snapshot.selection.project.id,"test",&task);
        if (status!=UMI_STATUS_OK) return status;
        status=append_task(&snapshot,&task);
        if (status!=UMI_STATUS_OK) return status;
    }

    needs_launch=(request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_RUN ||
                  request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_DEBUG ||
                  request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_FULL);
    if (needs_launch && !snapshot.selection.has_launch_profile)
        return UMI_STATUS_NOT_FOUND;

    if ((request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD ||
         request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_TEST ||
         request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_FULL) &&
        snapshot.task_count==0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    status=build_default_workflow_id(request,&snapshot.selection,
                                     workflow_id,sizeof(workflow_id));
    if (status!=UMI_STATUS_OK) return status;
    status=set_project_context(runtime,&snapshot.selection);
    if (status!=UMI_STATUS_OK) return status;

    for (index=0U;index<snapshot.task_count;++index)
        task_ids[index]=snapshot.task_ids[index];

    memset(&workflow_request,0,sizeof(workflow_request));
    workflow_request.struct_size=(uint32_t)sizeof(workflow_request);
    workflow_request.api_version=UMI_DEVELOPER_WORKFLOW_API_VERSION;
    workflow_request.workflow_id=workflow_id;
    workflow_request.task_ids=task_ids;
    workflow_request.task_count=snapshot.task_count;
    workflow_request.configuration_id=
        snapshot.selection.has_configuration?snapshot.selection.configuration.id:NULL;
    workflow_request.target_id=
        snapshot.selection.has_target?snapshot.selection.target.id:NULL;
    workflow_request.timeout_ms=request->timeout_ms;
    workflow_request.max_attempts=request->max_attempts;

    if (needs_launch) {
        workflow_request.launch_profile_id=snapshot.selection.launch_profile.id;
        snapshot.uses_launch_profile=1;
    }
    if (request->preset==UMI_DEVELOPER_PROJECT_WORKFLOW_DEBUG)
        workflow_request.debug_mode=1;
    else
        workflow_request.debug_mode=0;

    status=umi_developer_workflow_submit(runtime,&workflow_request,&snapshot.workflow);
    if (status!=UMI_STATUS_OK) return status;
    if (out_workflow!=NULL) *out_workflow=snapshot;
    return UMI_STATUS_OK;
}

/* ------------------------------------------------------------------------- */
/* Filesystem project bootstrap.                                              */
/* ------------------------------------------------------------------------- */

UmiStatus umi_developer_project_bootstrap(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot)
{
    UmiDeveloperProjectWorkflowRequest workflow_request;
    UmiStatus status;

    if (runtime == NULL || request == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

#define snapshot (*out_snapshot)
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
        if (snapshot.project.has_launch_profile)
            workflow_request.launch_profile_id = snapshot.project.launch_profile_id;

        status = umi_developer_project_workflow_submit(
            runtime, &workflow_request, &snapshot.workflow);
        if (status != UMI_STATUS_OK) return status;
        snapshot.workflow_prepared = 1;
    }

    status = umi_developer_context_snapshot(
        umi_developer_runtime_context(runtime), &snapshot.context);
    if (status != UMI_STATUS_OK) return status;

#undef snapshot
    return UMI_STATUS_OK;
}
