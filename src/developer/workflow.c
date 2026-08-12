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
