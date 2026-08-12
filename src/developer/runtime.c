/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/runtime.c
 *
 * PURPOSE:
 *   Implement the reusable developer orchestration runtime and publish its
 *   lifecycle into the product-neutral workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperRuntime {
    UmiDeveloperContext *context;
    UmiDeveloperPipeline *pipeline;
    UmiDeveloperJournal *journal;
    UmiProjectWorkspace *projects;
    UmiLanguageService *language;
    UmiDebugService *debug;
    UmiSourceControlService *source_control;
    UmiTestPlatformService *tests;
    UmiUiWorkbenchServices *workbench;
    uint64_t revision;
    int owns_services;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source == NULL) {
        source = "";
    }

    length = strlen(source);
    if (length >= capacity) {
        length = capacity - 1U;
    }
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
}

void umi_developer_runtime_bindings_init(UmiDeveloperRuntimeBindings *bindings)
{
    if (bindings == NULL) {
        return;
    }
    memset(bindings, 0, sizeof(*bindings));
    bindings->struct_size = (uint32_t)sizeof(*bindings);
    bindings->api_version = UMI_DEVELOPER_RUNTIME_API_VERSION;
}

static UmiStatus create_or_bind_common(
    UmiDeveloperRuntime *runtime)
{
    UmiStatus status;

    status = umi_developer_context_create(&runtime->context);
    if (status == UMI_STATUS_OK) {
        status = umi_developer_pipeline_create(&runtime->pipeline);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_journal_create(&runtime->journal);
    }
    return status;
}

UmiStatus umi_developer_runtime_create_owned(
    UmiDeveloperRuntime **out_runtime)
{
    UmiDeveloperRuntime *runtime;
    UmiStatus status = UMI_STATUS_OK;

    if (out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime = NULL;
    runtime = (UmiDeveloperRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    runtime->owns_services = 1;
    runtime->revision = 1U;

    if (status == UMI_STATUS_OK) status = umi_project_workspace_create(&runtime->projects);
    if (status == UMI_STATUS_OK) status = umi_language_service_create(&runtime->language);
    if (status == UMI_STATUS_OK) status = umi_debug_service_create(&runtime->debug);
    if (status == UMI_STATUS_OK) status = umi_source_control_service_create(&runtime->source_control);
    if (status == UMI_STATUS_OK) status = umi_test_platform_service_create(&runtime->tests);
    if (status == UMI_STATUS_OK) status = umi_ui_workbench_services_create(&runtime->workbench);
    if (status == UMI_STATUS_OK) status = create_or_bind_common(runtime);

    if (status != UMI_STATUS_OK) {
        umi_developer_runtime_destroy(runtime);
        return status;
    }

    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_runtime_create_bound(
    const UmiDeveloperRuntimeBindings *bindings,
    UmiDeveloperRuntime **out_runtime)
{
    UmiDeveloperRuntime *runtime;
    UmiStatus status;

    if (bindings == NULL || out_runtime == NULL ||
        bindings->projects == NULL ||
        bindings->language == NULL ||
        bindings->debug == NULL ||
        bindings->source_control == NULL ||
        bindings->tests == NULL ||
        bindings->workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime = NULL;
    runtime = (UmiDeveloperRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    runtime->projects = bindings->projects;
    runtime->language = bindings->language;
    runtime->debug = bindings->debug;
    runtime->source_control = bindings->source_control;
    runtime->tests = bindings->tests;
    runtime->workbench = bindings->workbench;
    runtime->owns_services = 0;
    runtime->revision = 1U;

    status = create_or_bind_common(runtime);
    if (status != UMI_STATUS_OK) {
        umi_developer_runtime_destroy(runtime);
        return status;
    }

    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

void umi_developer_runtime_destroy(UmiDeveloperRuntime *runtime)
{
    if (runtime == NULL) {
        return;
    }

    umi_developer_journal_destroy(runtime->journal);
    umi_developer_pipeline_destroy(runtime->pipeline);
    umi_developer_context_destroy(runtime->context);

    if (runtime->owns_services != 0) {
        umi_ui_workbench_services_destroy(runtime->workbench);
        umi_test_platform_service_destroy(runtime->tests);
        umi_source_control_service_destroy(runtime->source_control);
        umi_debug_service_destroy(runtime->debug);
        umi_language_service_destroy(runtime->language);
        umi_project_workspace_destroy(runtime->projects);
    }

    free(runtime);
}

static void publish_workbench(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperEventKind event_kind)
{
    UmiUiProgressSnapshot progress;
    UmiUiTaskMonitorSnapshot task;
    UmiUiStatusItemSnapshot status_item;
    UmiUiOutputChannelSnapshot output;
    UmiUiNotificationItemSnapshot notification;
    UmiUiProblemSnapshot problem;
    char status_text[UMI_DEVELOPER_SUMMARY_CAPACITY];

    if (runtime == NULL || runtime->workbench == NULL || operation == NULL) {
        return;
    }

    memset(&progress, 0, sizeof(progress));
    progress.struct_size = (uint32_t)sizeof(progress);
    progress.api_version = UMI_UI_PROGRESS_API_VERSION;
    copy_text(progress.id, sizeof(progress.id), operation->id);
    copy_text(progress.title, sizeof(progress.title), operation->title);
    copy_text(progress.detail, sizeof(progress.detail), operation->summary);
    progress.fraction = (double)operation->progress_basis_points / 10000.0;
    progress.state = (int)operation->state;
    progress.cancellable =
        operation->state == UMI_DEVELOPER_OPERATION_QUEUED ||
        operation->state == UMI_DEVELOPER_OPERATION_RUNNING;
    progress.indeterminate =
        operation->state == UMI_DEVELOPER_OPERATION_RUNNING &&
        operation->progress_basis_points == 0U;
    progress.revision = operation->revision;
    (void)umi_ui_progress_registry_upsert(
        umi_ui_workbench_services_progress(runtime->workbench), &progress);

    memset(&task, 0, sizeof(task));
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_UI_TASK_MONITOR_API_VERSION;
    copy_text(task.id, sizeof(task.id), operation->id);
    copy_text(task.label, sizeof(task.label), operation->title);
    copy_text(task.group, sizeof(task.group), "developer");
    copy_text(task.detail, sizeof(task.detail), operation->summary);
    task.state = (int)operation->state;
    task.background = 1;
    task.revision = operation->revision;
    (void)umi_ui_task_monitor_registry_upsert(
        umi_ui_workbench_services_task_monitor(runtime->workbench), &task);

    memset(&status_item, 0, sizeof(status_item));
    status_item.struct_size = (uint32_t)sizeof(status_item);
    status_item.api_version = UMI_UI_STATUS_ITEM_API_VERSION;
    copy_text(status_item.id, sizeof(status_item.id), "developer.operation.status");
    (void)snprintf(status_text, sizeof(status_text), "%s: %s",
                   operation->title,
                   umi_developer_operation_state_text(operation->state));
    copy_text(status_item.text, sizeof(status_item.text), status_text);
    copy_text(status_item.tooltip, sizeof(status_item.tooltip), operation->summary);
    copy_text(status_item.alignment, sizeof(status_item.alignment), "right");
    status_item.visible = 1;
    status_item.priority = 100;
    status_item.revision = operation->revision;
    (void)umi_ui_status_item_registry_upsert(
        umi_ui_workbench_services_status_item(runtime->workbench), &status_item);

    if (event_kind == UMI_DEVELOPER_EVENT_SUCCEEDED ||
        event_kind == UMI_DEVELOPER_EVENT_FAILED ||
        event_kind == UMI_DEVELOPER_EVENT_CANCELLED) {
        memset(&output, 0, sizeof(output));
        output.struct_size = (uint32_t)sizeof(output);
        output.api_version = UMI_UI_OUTPUT_CHANNEL_API_VERSION;
        copy_text(output.id, sizeof(output.id), operation->id);
        copy_text(output.name, sizeof(output.name), operation->title);
        copy_text(output.category, sizeof(output.category), "developer");
        copy_text(output.text, sizeof(output.text), operation->summary);
        output.sequence = operation->sequence;
        output.visible = 1;
        output.preserve = 1;
        output.revision = operation->revision;
        (void)umi_ui_output_channel_registry_upsert(
            umi_ui_workbench_services_output_channel(runtime->workbench), &output);
    }

    if (event_kind == UMI_DEVELOPER_EVENT_FAILED) {
        memset(&problem, 0, sizeof(problem));
        problem.struct_size = (uint32_t)sizeof(problem);
        problem.api_version = UMI_UI_PROBLEM_API_VERSION;
        copy_text(problem.id, sizeof(problem.id), operation->id);
        copy_text(problem.source, sizeof(problem.source), "developer");
        copy_text(problem.code, sizeof(problem.code), "operation.failed");
        copy_text(problem.message, sizeof(problem.message), operation->summary);
        copy_text(problem.uri, sizeof(problem.uri), operation->working_directory);
        problem.severity = 2;
        problem.resolved = 0;
        problem.revision = operation->revision;
        (void)umi_ui_problem_registry_upsert(
            umi_ui_workbench_services_problem(runtime->workbench), &problem);

        memset(&notification, 0, sizeof(notification));
        notification.struct_size = (uint32_t)sizeof(notification);
        notification.api_version = UMI_UI_NOTIFICATION_ITEM_API_VERSION;
        copy_text(notification.id, sizeof(notification.id), operation->id);
        copy_text(notification.title, sizeof(notification.title), operation->title);
        copy_text(notification.message, sizeof(notification.message), operation->summary);
        copy_text(notification.source, sizeof(notification.source), "developer");
        notification.severity = 2;
        notification.read = 0;
        notification.sticky = 1;
        notification.revision = operation->revision;
        (void)umi_ui_notification_item_registry_upsert(
            umi_ui_workbench_services_notification_item(runtime->workbench),
            &notification);
    } else if (event_kind == UMI_DEVELOPER_EVENT_SUCCEEDED ||
               event_kind == UMI_DEVELOPER_EVENT_RETRIED) {
        (void)umi_ui_problem_registry_remove(
            umi_ui_workbench_services_problem(runtime->workbench), operation->id);
    }
}

static UmiStatus record_operation_event(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    UmiDeveloperEventKind kind,
    const char *message)
{
    UmiDeveloperOperationSnapshot operation;
    UmiStatus status;

    status = umi_developer_pipeline_find(
        runtime->pipeline, operation_id, &operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_journal_append(
        runtime->journal, kind, operation_id, message, NULL);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    publish_workbench(runtime, &operation, kind);
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_runtime_set_context(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperContextSnapshot *context)
{
    UmiStatus status;

    if (runtime == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_context_set(runtime->context, context);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_journal_append(
        runtime->journal,
        UMI_DEVELOPER_EVENT_CONTEXT_CHANGED,
        "",
        "Developer context changed.",
        NULL);
    if (status == UMI_STATUS_OK) {
        runtime->revision += 1U;
    }
    return status;
}

UmiStatus umi_developer_runtime_submit_operation(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperOperationSnapshot *operation)
{
    UmiStatus status;

    if (runtime == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_submit(runtime->pipeline, operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return record_operation_event(
        runtime, operation->id, UMI_DEVELOPER_EVENT_SUBMITTED, "Operation queued.");
}

UmiStatus umi_developer_runtime_add_dependency(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *depends_on_operation_id)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_add_dependency(
        runtime->pipeline, operation_id, depends_on_operation_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return record_operation_event(
        runtime,
        operation_id,
        UMI_DEVELOPER_EVENT_DEPENDENCY_ADDED,
        "Operation dependency added.");
}

UmiStatus umi_developer_runtime_submit_cmake_plan(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan)
{
    UmiDeveloperCMakePlanSnapshot plan;
    const char *operation_ids[4];
    size_t index;
    UmiStatus status;

    if (runtime == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_cmake_plan_submit(runtime->pipeline, request, &plan);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    operation_ids[0] = plan.configure_operation_id;
    operation_ids[1] = plan.build_operation_id;
    operation_ids[2] = plan.test_operation_id;
    operation_ids[3] = plan.run_operation_id;

    for (index = 0U; index < 4U; ++index) {
        if (operation_ids[index][0] != '\0') {
            status = record_operation_event(
                runtime,
                operation_ids[index],
                UMI_DEVELOPER_EVENT_SUBMITTED,
                "Operation queued by CMake plan.");
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }
    }

    if (out_plan != NULL) {
        *out_plan = plan;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_runtime_start_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_start(runtime->pipeline, operation_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_STARTED, "Operation started.");
}

UmiStatus umi_developer_runtime_start_next(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperOperationSnapshot *out_operation)
{
    UmiDeveloperOperationSnapshot operation;
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_next_ready(runtime->pipeline, &operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_runtime_start_operation(runtime, operation.id);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_pipeline_find(runtime->pipeline, operation.id, &operation);
    if (status == UMI_STATUS_OK && out_operation != NULL) {
        *out_operation = operation;
    }
    return status;
}

UmiStatus umi_developer_runtime_set_progress(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_set_progress(
        runtime->pipeline, operation_id, progress_basis_points, summary);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_PROGRESS, summary);
}

UmiStatus umi_developer_runtime_complete_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_complete(
        runtime->pipeline, operation_id, exit_code, summary);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_SUCCEEDED, summary);
}

UmiStatus umi_developer_runtime_fail_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_fail(
        runtime->pipeline, operation_id, exit_code, summary);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_FAILED, summary);
}

UmiStatus umi_developer_runtime_cancel_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *summary)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_cancel(runtime->pipeline, operation_id, summary);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_CANCELLED, summary);
}

UmiStatus umi_developer_runtime_retry_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id)
{
    UmiStatus status;

    if (runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_retry(runtime->pipeline, operation_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return record_operation_event(
        runtime, operation_id, UMI_DEVELOPER_EVENT_RETRIED, "Operation queued for retry.");
}

UmiStatus umi_developer_runtime_execute_next(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result)
{
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperExecutionResult result;
    UmiStatus status;
    UmiStatus final_status;
    const char *summary;

    if (runtime == NULL || executor == NULL || executor->execute == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_runtime_start_next(runtime, &operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    status = executor->execute(executor->user_data, &operation, &result);
    summary = result.output[0] != '\0' ? result.output : umi_status_text(status);

    if (status == UMI_STATUS_CANCELLED || result.cancelled != 0) {
        final_status = umi_developer_runtime_cancel_operation(
            runtime, operation.id, summary);
    } else if (status == UMI_STATUS_OK && result.exit_code == 0 &&
               result.timed_out == 0) {
        final_status = umi_developer_runtime_complete_operation(
            runtime, operation.id, result.exit_code, summary);
    } else {
        final_status = umi_developer_runtime_fail_operation(
            runtime, operation.id, result.exit_code, summary);
    }

    if (final_status != UMI_STATUS_OK) {
        return final_status;
    }

    if (out_result != NULL) {
        *out_result = result;
    }

    final_status = umi_developer_pipeline_find(
        runtime->pipeline, operation.id, &operation);
    if (final_status == UMI_STATUS_OK && out_operation != NULL) {
        *out_operation = operation;
    }
    return final_status;
}

UmiStatus umi_developer_runtime_snapshot(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperRuntimeSnapshot *out_snapshot)
{
    UmiStatus status;

    if (runtime == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_RUNTIME_API_VERSION;
    out_snapshot->revision = runtime->revision;
    out_snapshot->owns_services = runtime->owns_services;

#define UMI_RUNTIME_SNAPSHOT(call_expression)                                      \
    do {                                                                           \
        status = (call_expression);                                                 \
        if (status != UMI_STATUS_OK) {                                              \
            return status;                                                          \
        }                                                                           \
    } while (0)

    UMI_RUNTIME_SNAPSHOT(umi_developer_context_snapshot(
        runtime->context, &out_snapshot->context));
    UMI_RUNTIME_SNAPSHOT(umi_developer_pipeline_snapshot(
        runtime->pipeline, &out_snapshot->pipeline));
    UMI_RUNTIME_SNAPSHOT(umi_developer_journal_snapshot(
        runtime->journal, &out_snapshot->journal));
    UMI_RUNTIME_SNAPSHOT(umi_project_workspace_snapshot(
        runtime->projects, &out_snapshot->projects));
    UMI_RUNTIME_SNAPSHOT(umi_language_service_snapshot(
        runtime->language, &out_snapshot->language));
    UMI_RUNTIME_SNAPSHOT(umi_debug_service_snapshot(
        runtime->debug, &out_snapshot->debug));
    UMI_RUNTIME_SNAPSHOT(umi_source_control_service_snapshot(
        runtime->source_control, &out_snapshot->source_control));
    UMI_RUNTIME_SNAPSHOT(umi_test_platform_service_snapshot(
        runtime->tests, &out_snapshot->tests));
    UMI_RUNTIME_SNAPSHOT(umi_ui_workbench_services_snapshot(
        runtime->workbench, &out_snapshot->workbench));

#undef UMI_RUNTIME_SNAPSHOT

    return UMI_STATUS_OK;
}

UmiDeveloperContext *umi_developer_runtime_context(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->context : NULL;
}

UmiDeveloperPipeline *umi_developer_runtime_pipeline(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->pipeline : NULL;
}

UmiDeveloperJournal *umi_developer_runtime_journal(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->journal : NULL;
}

UmiProjectWorkspace *umi_developer_runtime_projects(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->projects : NULL;
}

UmiLanguageService *umi_developer_runtime_language(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->language : NULL;
}

UmiDebugService *umi_developer_runtime_debug(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->debug : NULL;
}

UmiSourceControlService *umi_developer_runtime_source_control(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->source_control : NULL;
}

UmiTestPlatformService *umi_developer_runtime_tests(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->tests : NULL;
}

UmiUiWorkbenchServices *umi_developer_runtime_workbench(UmiDeveloperRuntime *runtime)
{
    return runtime != NULL ? runtime->workbench : NULL;
}
