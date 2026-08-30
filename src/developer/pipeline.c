/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/pipeline.c
 *
 * PURPOSE:
 *   Implement deterministic developer-operation scheduling and lifecycle rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/pipeline.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiDeveloperDependency {
    size_t operation_index;
    size_t depends_on_index;
} UmiDeveloperDependency;

struct UmiDeveloperPipeline {
    UmiDeveloperOperationSnapshot *operations;
    UmiDeveloperDependency *dependencies;
    size_t operation_count;
    size_t dependency_count;
    uint64_t next_sequence;
    uint64_t revision;
};

static void terminate_operation(UmiDeveloperOperationSnapshot *operation)
{
    size_t index;

    operation->id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->title[UMI_DEVELOPER_TITLE_CAPACITY - 1U] = '\0';
    operation->project_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->configuration_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->target_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->program[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
    operation->working_directory[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
    operation->summary[UMI_DEVELOPER_SUMMARY_CAPACITY - 1U] = '\0';

    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        operation->argument_count = UMI_DEVELOPER_MAX_ARGUMENTS;
    }
    for (index = 0U; index < operation->argument_count; ++index) {
        operation->arguments[index][UMI_DEVELOPER_ARGUMENT_CAPACITY - 1U] = '\0';
    }

    operation->struct_size = (uint32_t)sizeof(*operation);
    operation->api_version = UMI_DEVELOPER_OPERATION_API_VERSION;
    if (operation->max_attempts == 0U) {
        operation->max_attempts = 1U;
    }
}

static UmiStatus copy_summary(
    UmiDeveloperOperationSnapshot *operation,
    const char *summary)
{
    size_t length;

    if (operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (summary == NULL) {
        operation->summary[0] = '\0';
        return UMI_STATUS_OK;
    }

    length = strlen(summary);
    if (length >= sizeof(operation->summary)) {
        length = sizeof(operation->summary) - 1U;
    }
    if (length > 0U) {
        memcpy(operation->summary, summary, length);
    }
    operation->summary[length] = '\0';
    return UMI_STATUS_OK;
}

static int find_index(
    const UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    size_t *out_index)
{
    size_t index;

    if (pipeline == NULL || operation_id == NULL || operation_id[0] == '\0') {
        return 0;
    }

    for (index = 0U; index < pipeline->operation_count; ++index) {
        if (strcmp(pipeline->operations[index].id, operation_id) == 0) {
            if (out_index != NULL) {
                *out_index = index;
            }
            return 1;
        }
    }
    return 0;
}

static int dependency_exists(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index,
    size_t depends_on_index)
{
    size_t index;

    for (index = 0U; index < pipeline->dependency_count; ++index) {
        if (pipeline->dependencies[index].operation_index == operation_index &&
            pipeline->dependencies[index].depends_on_index == depends_on_index) {
            return 1;
        }
    }
    return 0;
}

static int transitively_depends_on(
    const UmiDeveloperPipeline *pipeline,
    size_t start_index,
    size_t target_index)
{
    size_t stack[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    unsigned char visited[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    size_t stack_count = 0U;

    memset(visited, 0, sizeof(visited));
    stack[stack_count++] = start_index;

    while (stack_count > 0U) {
        size_t current = stack[--stack_count];
        size_t dependency_index;

        if (current == target_index) {
            return 1;
        }
        if (visited[current] != 0U) {
            continue;
        }
        visited[current] = 1U;

        for (dependency_index = 0U;
             dependency_index < pipeline->dependency_count;
             ++dependency_index) {
            const UmiDeveloperDependency *dependency =
                &pipeline->dependencies[dependency_index];

            if (dependency->operation_index == current &&
                visited[dependency->depends_on_index] == 0U) {
                if (stack_count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
                    return 1;
                }
                stack[stack_count++] = dependency->depends_on_index;
            }
        }
    }

    return 0;
}

static int dependencies_succeeded(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index)
{
    size_t index;

    for (index = 0U; index < pipeline->dependency_count; ++index) {
        const UmiDeveloperDependency *dependency = &pipeline->dependencies[index];
        if (dependency->operation_index == operation_index &&
            pipeline->operations[dependency->depends_on_index].state !=
                UMI_DEVELOPER_OPERATION_SUCCEEDED) {
            return 0;
        }
    }
    return 1;
}

static int dependency_terminal_failure(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index)
{
    size_t index;

    for (index = 0U; index < pipeline->dependency_count; ++index) {
        const UmiDeveloperDependency *dependency = &pipeline->dependencies[index];
        UmiDeveloperOperationState state;

        if (dependency->operation_index != operation_index) {
            continue;
        }

        state = pipeline->operations[dependency->depends_on_index].state;
        if (state == UMI_DEVELOPER_OPERATION_FAILED ||
            state == UMI_DEVELOPER_OPERATION_CANCELLED ||
            state == UMI_DEVELOPER_OPERATION_BLOCKED) {
            return 1;
        }
    }
    return 0;
}

UmiStatus umi_developer_pipeline_create(UmiDeveloperPipeline **out_pipeline)
{
    UmiDeveloperPipeline *pipeline;

    if (out_pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_pipeline = NULL;
    pipeline = (UmiDeveloperPipeline *)calloc(1U, sizeof(*pipeline));
    if (pipeline == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    pipeline->operations = (UmiDeveloperOperationSnapshot *)calloc(
        UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY,
        sizeof(*pipeline->operations));
    pipeline->dependencies = (UmiDeveloperDependency *)calloc(
        UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY,
        sizeof(*pipeline->dependencies));

    if (pipeline->operations == NULL || pipeline->dependencies == NULL) {
        umi_developer_pipeline_destroy(pipeline);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    pipeline->next_sequence = 1U;
    pipeline->revision = 1U;
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

void umi_developer_pipeline_destroy(UmiDeveloperPipeline *pipeline)
{
    if (pipeline == NULL) {
        return;
    }
    free(pipeline->dependencies);
    free(pipeline->operations);
    free(pipeline);
}

UmiStatus umi_developer_pipeline_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperOperationSnapshot *operation)
{
    UmiDeveloperOperationSnapshot copy;

    if (pipeline == NULL || operation == NULL || operation->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (pipeline->operation_count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (find_index(pipeline, operation->id, NULL)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *operation;
    terminate_operation(&copy);
    copy.state = UMI_DEVELOPER_OPERATION_QUEUED;
    copy.attempt_count = 0U;
    copy.exit_code = 0;
    copy.has_exit_code = 0;
    copy.progress_basis_points = 0U;
    copy.sequence = pipeline->next_sequence++;
    copy.revision = 1U;

    pipeline->operations[pipeline->operation_count++] = copy;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_pipeline_add_dependency(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *depends_on_operation_id)
{
    size_t operation_index;
    size_t depends_on_index;

    if (pipeline == NULL || operation_id == NULL ||
        depends_on_operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &operation_index) ||
        !find_index(pipeline, depends_on_operation_id, &depends_on_index)) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (operation_index == depends_on_index) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (dependency_exists(pipeline, operation_index, depends_on_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (pipeline->dependency_count >=
        UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * operation -> dependency is a backwards edge.  Adding it is cyclic when
     * the proposed dependency already (directly or indirectly) depends on the
     * operation.
     */
    if (transitively_depends_on(pipeline, depends_on_index, operation_index)) {
        return UMI_STATUS_INVALID_STATE;
    }

    pipeline->dependencies[pipeline->dependency_count].operation_index =
        operation_index;
    pipeline->dependencies[pipeline->dependency_count].depends_on_index =
        depends_on_index;
    pipeline->dependency_count += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}

UmiStatus umi_developer_pipeline_find(
    const UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    UmiDeveloperOperationSnapshot *out_operation)
{
    size_t index;

    if (pipeline == NULL || operation_id == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_pipeline_at(
    const UmiDeveloperPipeline *pipeline,
    size_t index,
    UmiDeveloperOperationSnapshot *out_operation)
{
    if (pipeline == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= pipeline->operation_count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[index];
    return UMI_STATUS_OK;
}

size_t umi_developer_pipeline_count(const UmiDeveloperPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->operation_count : 0U;
}

uint64_t umi_developer_pipeline_revision(const UmiDeveloperPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->revision : 0U;
}

UmiStatus umi_developer_pipeline_refresh(UmiDeveloperPipeline *pipeline)
{
    size_t index;
    int changed = 0;

    if (pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Repeat until stable because changing one blocked node can affect another
     * node later in a dependency chain.
     */
    for (;;) {
        int pass_changed = 0;

        for (index = 0U; index < pipeline->operation_count; ++index) {
            UmiDeveloperOperationSnapshot *operation =
                &pipeline->operations[index];

            if (operation->state == UMI_DEVELOPER_OPERATION_QUEUED &&
                dependency_terminal_failure(pipeline, index)) {
                operation->state = UMI_DEVELOPER_OPERATION_BLOCKED;
                operation->revision += 1U;
                pass_changed = 1;
            } else if (operation->state == UMI_DEVELOPER_OPERATION_BLOCKED &&
                       !dependency_terminal_failure(pipeline, index)) {
                operation->state = UMI_DEVELOPER_OPERATION_QUEUED;
                operation->revision += 1U;
                pass_changed = 1;
            }
        }

        if (pass_changed == 0) {
            break;
        }
        changed = 1;
    }

    if (changed != 0) {
        pipeline->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_pipeline_next_ready(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperOperationSnapshot *out_operation)
{
    size_t index;
    size_t best_index = 0U;
    uint64_t best_sequence = UINT64_MAX;
    int found = 0;
    UmiStatus status;

    if (pipeline == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    for (index = 0U; index < pipeline->operation_count; ++index) {
        const UmiDeveloperOperationSnapshot *operation =
            &pipeline->operations[index];

        if (operation->state == UMI_DEVELOPER_OPERATION_QUEUED &&
            dependencies_succeeded(pipeline, index) &&
            operation->sequence < best_sequence) {
            best_index = index;
            best_sequence = operation->sequence;
            found = 1;
        }
    }

    if (found == 0) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[best_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_pipeline_start(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;
    UmiStatus status;

    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    if (operation->state != UMI_DEVELOPER_OPERATION_QUEUED ||
        !dependencies_succeeded(pipeline, index)) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (operation->attempt_count >= operation->max_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_RUNNING;
    operation->attempt_count += 1U;
    operation->progress_basis_points = 0U;
    operation->has_exit_code = 0;
    operation->revision += 1U;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_pipeline_set_progress(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    if (pipeline == NULL || operation_id == NULL ||
        progress_basis_points > 10000U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    if (operation->state != UMI_DEVELOPER_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->progress_basis_points = progress_basis_points;
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus finish_operation(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    UmiDeveloperOperationState state,
    int exit_code,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    if (operation->state != UMI_DEVELOPER_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = state;
    operation->exit_code = exit_code;
    operation->has_exit_code = 1;
    if (state == UMI_DEVELOPER_OPERATION_SUCCEEDED) {
        operation->progress_basis_points = 10000U;
    }
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}

UmiStatus umi_developer_pipeline_complete(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    return finish_operation(
        pipeline,
        operation_id,
        UMI_DEVELOPER_OPERATION_SUCCEEDED,
        exit_code,
        summary);
}

UmiStatus umi_developer_pipeline_fail(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    return finish_operation(
        pipeline,
        operation_id,
        UMI_DEVELOPER_OPERATION_FAILED,
        exit_code,
        summary);
}

UmiStatus umi_developer_pipeline_cancel(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    if (operation->state != UMI_DEVELOPER_OPERATION_QUEUED &&
        operation->state != UMI_DEVELOPER_OPERATION_RUNNING &&
        operation->state != UMI_DEVELOPER_OPERATION_BLOCKED) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_CANCELLED;
    operation->has_exit_code = 0;
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}

UmiStatus umi_developer_pipeline_retry(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    if (operation->state != UMI_DEVELOPER_OPERATION_FAILED &&
        operation->state != UMI_DEVELOPER_OPERATION_CANCELLED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (operation->attempt_count >= operation->max_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_QUEUED;
    operation->progress_basis_points = 0U;
    operation->exit_code = 0;
    operation->has_exit_code = 0;
    operation->summary[0] = '\0';
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}

UmiStatus umi_developer_pipeline_snapshot(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperPipelineSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus status;

    if (pipeline == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_PIPELINE_API_VERSION;
    out_snapshot->operation_count = pipeline->operation_count;
    out_snapshot->dependency_count = pipeline->dependency_count;
    out_snapshot->revision = pipeline->revision;

    for (index = 0U; index < pipeline->operation_count; ++index) {
        UmiDeveloperOperationState state = pipeline->operations[index].state;
        switch (state) {
            case UMI_DEVELOPER_OPERATION_QUEUED:
                out_snapshot->queued_count += 1U;
                if (dependencies_succeeded(pipeline, index)) {
                    out_snapshot->ready_count += 1U;
                }
                break;
            case UMI_DEVELOPER_OPERATION_RUNNING:
                out_snapshot->running_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_SUCCEEDED:
                out_snapshot->succeeded_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_FAILED:
                out_snapshot->failed_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_CANCELLED:
                out_snapshot->cancelled_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_BLOCKED:
                out_snapshot->blocked_count += 1U;
                break;
            default:
                return UMI_STATUS_INTERNAL_ERROR;
        }
    }

    return UMI_STATUS_OK;
}
