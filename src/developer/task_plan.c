/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/task_plan.c
 *
 * PURPOSE:
 *   Implement project-task translation into the common developer pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/task_plan.h"

#include <stdio.h>
#include <string.h>

#include "umicom/developer/command_line.h"

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_task_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan)
{
    UmiProjectTaskSnapshot task;
    UmiDeveloperCommandLine command;
    UmiDeveloperOperationSnapshot operation;
    UmiProjectTaskRegistry *registry;
    char generated_id[UMI_DEVELOPER_ID_CAPACITY];
    const char *operation_id;
    size_t index;
    UmiStatus status;

    if (runtime == NULL || request == NULL || request->task_id == NULL ||
        request->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_project_workspace_task(umi_developer_runtime_projects(runtime));
    if (registry == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_project_task_registry_find(registry, request->task_id, &task);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (task.enabled == 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_developer_command_line_parse(task.command, &command);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    operation_id = request->operation_id;
    if (operation_id == NULL || operation_id[0] == '\0') {
        int written = snprintf(generated_id, sizeof(generated_id), "task.%s", task.id);
        if (written < 0 || (size_t)written >= sizeof(generated_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        operation_id = generated_id;
    }

    status = umi_developer_operation_init(
        &operation,
        operation_id,
        UMI_DEVELOPER_OPERATION_CUSTOM,
        task.label[0] != '\0' ? task.label : task.id);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_operation_set_program(
        &operation,
        command.program,
        task.working_directory[0] != '\0' ? task.working_directory : NULL);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    for (index = 0U; index < command.argument_count; ++index) {
        status = umi_developer_operation_add_argument(
            &operation, command.arguments[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = copy_text(operation.project_id, sizeof(operation.project_id), task.project_id);
    if (status != UMI_STATUS_OK) return status;
    if (request->configuration_id != NULL) {
        status = copy_text(operation.configuration_id,
                           sizeof(operation.configuration_id),
                           request->configuration_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (request->target_id != NULL) {
        status = copy_text(operation.target_id,
                           sizeof(operation.target_id),
                           request->target_id);
        if (status != UMI_STATUS_OK) return status;
    }
    operation.timeout_ms = request->timeout_ms;
    operation.max_attempts = request->max_attempts == 0U ? 1U : request->max_attempts;

    status = umi_developer_runtime_submit_operation(runtime, &operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    if (out_plan != NULL) {
        memset(out_plan, 0, sizeof(*out_plan));
        out_plan->struct_size = (uint32_t)sizeof(*out_plan);
        out_plan->api_version = UMI_DEVELOPER_TASK_PLAN_API_VERSION;
        (void)copy_text(out_plan->operation_id, sizeof(out_plan->operation_id), operation.id);
        (void)copy_text(out_plan->task_id, sizeof(out_plan->task_id), task.id);
        (void)copy_text(out_plan->project_id, sizeof(out_plan->project_id), task.project_id);
        (void)copy_text(out_plan->group, sizeof(out_plan->group), task.group);
        out_plan->background = task.background;
    }
    return UMI_STATUS_OK;
}
