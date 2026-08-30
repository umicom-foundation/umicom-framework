/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/task_orchestrator.c
 *
 * PURPOSE:
 *   Implement dependency-closed task planning and cooperative orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/task_orchestrator.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildTaskOrchestrator {
    const UmiBuildTaskRegistry *source_registry;
    UmiBuildTaskRegistry *planned_registry;
    UmiBuildTaskGraph *graph;
    char operation_id[UMI_BUILD_ID_CAPACITY];
    char requested_task_id[UMI_BUILD_ID_CAPACITY];
    UmiBuildTaskOperationState state;
    size_t maximum_parallel;
    size_t active_count;
    size_t retry_count;
    uint64_t generation;
    uint64_t revision;
    int stop_requested;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t source_index(const UmiBuildTaskRegistry *registry,
                           const char *task_id)
{
    size_t index;
    size_t count = umi_build_task_registry_count(registry);
    for (index = 0U; index < count; ++index) {
        UmiBuildTaskSnapshot task;
        if (umi_build_task_registry_at(registry, index, &task) == UMI_STATUS_OK &&
            strcmp(task.task_id, task_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static UmiStatus include_task_recursive(
    UmiBuildTaskOrchestrator *orchestrator,
    size_t index,
    unsigned char *visiting,
    unsigned char *included)
{
    UmiBuildTaskSnapshot task;
    size_t dependency;
    UmiStatus status;
    if (index >= umi_build_task_registry_count(orchestrator->source_registry)) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (included[index] != 0U) return UMI_STATUS_OK;
    if (visiting[index] != 0U) return UMI_STATUS_INVALID_STATE;
    visiting[index] = 1U;
    status = umi_build_task_registry_at(
        orchestrator->source_registry, index, &task);
    if (status != UMI_STATUS_OK) return status;
    for (dependency = 0U; dependency < task.dependency_count; ++dependency) {
        const size_t dependency_index = source_index(
            orchestrator->source_registry, task.dependencies[dependency]);
        if (dependency_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
        status = include_task_recursive(orchestrator, dependency_index,
                                        visiting, included);
        if (status != UMI_STATUS_OK) return status;
    }
    visiting[index] = 0U;
    included[index] = 1U;
    return umi_build_task_registry_upsert(orchestrator->planned_registry, &task);
}

static void update_terminal_state(UmiBuildTaskOrchestrator *orchestrator)
{
    UmiBuildTaskGraphSnapshot graph;
    if (umi_build_task_graph_snapshot(orchestrator->graph, &graph) !=
        UMI_STATUS_OK) {
        return;
    }
    if (graph.running_count > 0U || graph.ready_count > 0U ||
        graph.pending_count > 0U) {
        return;
    }
    if (orchestrator->stop_requested || graph.cancelled_count > 0U) {
        orchestrator->state = UMI_BUILD_TASK_OPERATION_CANCELLED;
    } else if (graph.failed_count > 0U || graph.blocked_count > 0U) {
        orchestrator->state = UMI_BUILD_TASK_OPERATION_FAILED;
    } else {
        orchestrator->state = UMI_BUILD_TASK_OPERATION_SUCCEEDED;
    }
}

UmiStatus umi_build_task_orchestrator_create(
    const UmiBuildTaskRegistry *source_registry,
    size_t maximum_parallel,
    UmiBuildTaskOrchestrator **out_orchestrator)
{
    UmiBuildTaskOrchestrator *orchestrator;
    UmiStatus status;
    if (source_registry == NULL || out_orchestrator == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_orchestrator = NULL;
    orchestrator = (UmiBuildTaskOrchestrator *)calloc(1U, sizeof(*orchestrator));
    if (orchestrator == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    orchestrator->source_registry = source_registry;
    orchestrator->maximum_parallel = maximum_parallel == 0U ? 1U
                                                             : maximum_parallel;
    orchestrator->state = UMI_BUILD_TASK_OPERATION_IDLE;
    orchestrator->revision = 1U;
    status = umi_build_task_registry_create(&orchestrator->planned_registry);
    if (status == UMI_STATUS_OK) {
        status = umi_build_task_graph_create(&orchestrator->graph);
    }
    if (status != UMI_STATUS_OK) {
        umi_build_task_orchestrator_destroy(orchestrator);
        return status;
    }
    *out_orchestrator = orchestrator;
    return UMI_STATUS_OK;
}

void umi_build_task_orchestrator_destroy(
    UmiBuildTaskOrchestrator *orchestrator)
{
    if (orchestrator == NULL) return;
    umi_build_task_graph_destroy(orchestrator->graph);
    umi_build_task_registry_destroy(orchestrator->planned_registry);
    free(orchestrator);
}

UmiStatus umi_build_task_orchestrator_plan(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *operation_id,
    const char *requested_task_id)
{
    unsigned char visiting[UMI_BUILD_TASK_CAPACITY];
    unsigned char included[UMI_BUILD_TASK_CAPACITY];
    size_t count;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (orchestrator == NULL || operation_id == NULL ||
        operation_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (orchestrator->state == UMI_BUILD_TASK_OPERATION_RUNNING) {
        return UMI_STATUS_BUSY;
    }
    (void)memset(visiting, 0, sizeof(visiting));
    (void)memset(included, 0, sizeof(included));
    umi_build_task_registry_clear(orchestrator->planned_registry);
    count = umi_build_task_registry_count(orchestrator->source_registry);
    if (requested_task_id == NULL || requested_task_id[0] == '\0') {
        for (index = 0U; index < count; ++index) {
            UmiBuildTaskSnapshot task;
            status = umi_build_task_registry_at(
                orchestrator->source_registry, index, &task);
            if (status != UMI_STATUS_OK) break;
            if ((task.flags & UMI_BUILD_TASK_ENABLED) == 0U) continue;
            status = include_task_recursive(orchestrator, index,
                                            visiting, included);
            if (status != UMI_STATUS_OK) break;
        }
    } else {
        index = source_index(orchestrator->source_registry, requested_task_id);
        if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
        status = include_task_recursive(orchestrator, index, visiting, included);
    }
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_task_graph_rebuild(
        orchestrator->graph, orchestrator->planned_registry);
    if (status != UMI_STATUS_OK) return status;
    copy_text(orchestrator->operation_id,
              sizeof(orchestrator->operation_id), operation_id);
    copy_text(orchestrator->requested_task_id,
              sizeof(orchestrator->requested_task_id), requested_task_id);
    orchestrator->state = UMI_BUILD_TASK_OPERATION_PLANNED;
    orchestrator->active_count = 0U;
    orchestrator->retry_count = 0U;
    orchestrator->stop_requested = 0;
    orchestrator->generation += 1U;
    orchestrator->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_orchestrator_begin(
    UmiBuildTaskOrchestrator *orchestrator)
{
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (orchestrator->state != UMI_BUILD_TASK_OPERATION_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestrator->state = UMI_BUILD_TASK_OPERATION_RUNNING;
    orchestrator->revision += 1U;
    update_terminal_state(orchestrator);
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_orchestrator_next_ready(
    UmiBuildTaskOrchestrator *orchestrator,
    UmiBuildTaskSnapshot *out_task)
{
    UmiBuildTaskGraphNodeSnapshot node;
    UmiStatus status;
    if (orchestrator == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (orchestrator->state != UMI_BUILD_TASK_OPERATION_RUNNING ||
        orchestrator->stop_requested) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (orchestrator->active_count >= orchestrator->maximum_parallel) {
        return UMI_STATUS_BUSY;
    }
    status = umi_build_task_graph_next_ready(orchestrator->graph, &node);
    if (status != UMI_STATUS_OK) return status;
    *out_task = node.task;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_orchestrator_start(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id)
{
    UmiStatus status;
    if (orchestrator == NULL || task_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (orchestrator->state != UMI_BUILD_TASK_OPERATION_RUNNING ||
        orchestrator->stop_requested ||
        orchestrator->active_count >= orchestrator->maximum_parallel) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_task_graph_start(orchestrator->graph, task_id);
    if (status == UMI_STATUS_OK) {
        orchestrator->active_count += 1U;
        orchestrator->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_task_orchestrator_set_progress(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    uint32_t progress_basis_points)
{
    UmiStatus status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_task_graph_set_progress(
        orchestrator->graph, task_id, progress_basis_points);
    if (status == UMI_STATUS_OK) orchestrator->revision += 1U;
    return status;
}

UmiStatus umi_build_task_orchestrator_complete(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    int exit_code)
{
    UmiStatus status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_task_graph_complete(
        orchestrator->graph, task_id, exit_code);
    if (status == UMI_STATUS_OK) {
        if (orchestrator->active_count > 0U) orchestrator->active_count -= 1U;
        orchestrator->revision += 1U;
        update_terminal_state(orchestrator);
    }
    return status;
}

UmiStatus umi_build_task_orchestrator_fail(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    UmiStatus status,
    int exit_code)
{
    UmiStatus graph_status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    graph_status = umi_build_task_graph_fail(
        orchestrator->graph, task_id, status, exit_code);
    if (graph_status == UMI_STATUS_OK) {
        if (orchestrator->active_count > 0U) orchestrator->active_count -= 1U;
        orchestrator->revision += 1U;
        update_terminal_state(orchestrator);
    }
    return graph_status;
}

UmiStatus umi_build_task_orchestrator_retry(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id)
{
    UmiStatus status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_task_graph_retry(orchestrator->graph, task_id);
    if (status == UMI_STATUS_OK) {
        orchestrator->retry_count += 1U;
        orchestrator->stop_requested = 0;
        orchestrator->state = UMI_BUILD_TASK_OPERATION_RUNNING;
        orchestrator->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_task_orchestrator_request_stop(
    UmiBuildTaskOrchestrator *orchestrator)
{
    UmiStatus status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (orchestrator->state != UMI_BUILD_TASK_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestrator->stop_requested = 1;
    status = umi_build_task_graph_cancel_waiting(orchestrator->graph);
    if (status != UMI_STATUS_OK) return status;
    if (orchestrator->active_count == 0U) {
        orchestrator->state = UMI_BUILD_TASK_OPERATION_CANCELLED;
    }
    orchestrator->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_orchestrator_finish(
    UmiBuildTaskOrchestrator *orchestrator)
{
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (orchestrator->active_count != 0U) return UMI_STATUS_BUSY;
    update_terminal_state(orchestrator);
    if (orchestrator->state == UMI_BUILD_TASK_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestrator->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_orchestrator_snapshot(
    const UmiBuildTaskOrchestrator *orchestrator,
    UmiBuildTaskOperationSnapshot *out_snapshot)
{
    UmiStatus status;
    if (orchestrator == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_TASK_ORCHESTRATOR_API_VERSION;
    copy_text(out_snapshot->operation_id, sizeof(out_snapshot->operation_id),
              orchestrator->operation_id);
    copy_text(out_snapshot->requested_task_id,
              sizeof(out_snapshot->requested_task_id),
              orchestrator->requested_task_id);
    out_snapshot->state = orchestrator->state;
    out_snapshot->maximum_parallel = orchestrator->maximum_parallel;
    out_snapshot->active_count = orchestrator->active_count;
    out_snapshot->retry_count = orchestrator->retry_count;
    out_snapshot->generation = orchestrator->generation;
    out_snapshot->revision = orchestrator->revision;
    out_snapshot->stop_requested = orchestrator->stop_requested;
    status = umi_build_task_graph_snapshot(orchestrator->graph,
                                           &out_snapshot->graph);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->planned_count = out_snapshot->graph.task_count;
    out_snapshot->completed_count =
        out_snapshot->graph.succeeded_count +
        out_snapshot->graph.failed_count +
        out_snapshot->graph.cancelled_count +
        out_snapshot->graph.skipped_count;
    return UMI_STATUS_OK;
}

UmiBuildTaskGraph *umi_build_task_orchestrator_graph(
    UmiBuildTaskOrchestrator *orchestrator)
{
    return orchestrator != NULL ? orchestrator->graph : NULL;
}

const char *umi_build_task_operation_state_text(
    UmiBuildTaskOperationState state)
{
    switch (state) {
    case UMI_BUILD_TASK_OPERATION_IDLE: return "Idle";
    case UMI_BUILD_TASK_OPERATION_PLANNED: return "Planned";
    case UMI_BUILD_TASK_OPERATION_RUNNING: return "Running";
    case UMI_BUILD_TASK_OPERATION_SUCCEEDED: return "Succeeded";
    case UMI_BUILD_TASK_OPERATION_FAILED: return "Failed";
    case UMI_BUILD_TASK_OPERATION_CANCELLED: return "Cancelled";
    default: return "Unknown";
    }
}
