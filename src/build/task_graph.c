/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/task_graph.c
 *
 * PURPOSE:
 *   Implement dependency-aware task readiness, terminal-state propagation,
 *   retries, cancellation and aggregate progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/task_graph.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildTaskGraph {
    UmiBuildTaskGraphNodeSnapshot nodes[UMI_BUILD_TASK_CAPACITY];
    size_t count;
    size_t dependency_count;
    uint64_t source_revision;
    uint64_t revision;
    int cyclic;
};

static size_t find_index(const UmiBuildTaskGraph *graph, const char *task_id)
{
    size_t index;
    if (graph == NULL || task_id == NULL) return SIZE_MAX;
    for (index = 0U; index < graph->count; ++index) {
        if (strcmp(graph->nodes[index].task.task_id, task_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int is_success_like(UmiBuildTaskState state)
{
    return state == UMI_BUILD_TASK_SUCCEEDED ||
           state == UMI_BUILD_TASK_SKIPPED;
}

static int is_failure_like(UmiBuildTaskState state)
{
    return state == UMI_BUILD_TASK_FAILED ||
           state == UMI_BUILD_TASK_CANCELLED ||
           state == UMI_BUILD_TASK_BLOCKED;
}

static int dependency_allows_failure(
    const UmiBuildTaskGraphNodeSnapshot *dependency)
{
    return dependency != NULL &&
           (dependency->task.flags & UMI_BUILD_TASK_CONTINUE_ON_ERROR) != 0U;
}

static UmiStatus detect_cycle(UmiBuildTaskGraph *graph)
{
    size_t indegree[UMI_BUILD_TASK_CAPACITY];
    size_t queue[UMI_BUILD_TASK_CAPACITY];
    size_t begin = 0U;
    size_t end = 0U;
    size_t visited = 0U;
    size_t index;

    (void)memset(indegree, 0, sizeof(indegree));
    for (index = 0U; index < graph->count; ++index) {
        const UmiBuildTaskSnapshot *task = &graph->nodes[index].task;
        size_t dependency;
        if ((task->flags & UMI_BUILD_TASK_ENABLED) == 0U) continue;
        for (dependency = 0U; dependency < task->dependency_count; ++dependency) {
            const size_t dependency_index =
                find_index(graph, task->dependencies[dependency]);
            if (dependency_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
            if ((graph->nodes[dependency_index].task.flags &
                 UMI_BUILD_TASK_ENABLED) != 0U) {
                indegree[index] += 1U;
            }
        }
    }
    for (index = 0U; index < graph->count; ++index) {
        if ((graph->nodes[index].task.flags & UMI_BUILD_TASK_ENABLED) != 0U &&
            indegree[index] == 0U) {
            queue[end++] = index;
        }
    }
    while (begin < end) {
        const size_t dependency_index = queue[begin++];
        size_t consumer;
        visited += 1U;
        for (consumer = 0U; consumer < graph->count; ++consumer) {
            UmiBuildTaskSnapshot *task = &graph->nodes[consumer].task;
            size_t position;
            if ((task->flags & UMI_BUILD_TASK_ENABLED) == 0U) continue;
            for (position = 0U; position < task->dependency_count; ++position) {
                if (strcmp(task->dependencies[position],
                           graph->nodes[dependency_index].task.task_id) == 0) {
                    if (indegree[consumer] > 0U) indegree[consumer] -= 1U;
                    if (indegree[consumer] == 0U) queue[end++] = consumer;
                }
            }
        }
    }
    {
        size_t enabled_count = 0U;
        for (index = 0U; index < graph->count; ++index) {
            if ((graph->nodes[index].task.flags & UMI_BUILD_TASK_ENABLED) != 0U) {
                enabled_count += 1U;
            }
        }
        graph->cyclic = visited != enabled_count;
    }
    return graph->cyclic ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}


/* Runtime transitions stage only the changing fields, not the large copied
 * task descriptions. Publication has no remaining failure points once every
 * dependency and revision has been admitted. Owners serialize graph access. */
typedef struct TaskGraphRuntime {
    UmiBuildTaskState state;
    size_t satisfied;
    size_t failed;
    uint32_t progress;
    uint32_t attempts;
    UmiStatus status;
    int exit_code;
    uint64_t revision;
} TaskGraphRuntime;

static void runtime_capture(const UmiBuildTaskGraph *graph, TaskGraphRuntime *states)
{
    for (size_t i = 0U; i < graph->count; ++i) {
        const UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[i];
        states[i] = (TaskGraphRuntime){node->state, node->satisfied_dependency_count,
            node->failed_dependency_count, node->progress_basis_points,
            node->attempt_count, node->last_status, node->last_exit_code, node->revision};
    }
}

static UmiStatus runtime_advance(TaskGraphRuntime *state)
{
    if (state->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    state->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus runtime_refresh(const UmiBuildTaskGraph *graph, TaskGraphRuntime *states)
{
    if (graph->cyclic) return UMI_STATUS_INVALID_STATE;
    int changed;
    do {
        changed = 0;
        for (size_t i = 0U; i < graph->count; ++i) {
            TaskGraphRuntime *node = &states[i];
            const UmiBuildTaskSnapshot *task = &graph->nodes[i].task;
            if (node->state == UMI_BUILD_TASK_RUNNING || node->state == UMI_BUILD_TASK_SUCCEEDED ||
                node->state == UMI_BUILD_TASK_FAILED || node->state == UMI_BUILD_TASK_CANCELLED ||
                node->state == UMI_BUILD_TASK_SKIPPED) continue;
            size_t satisfied = 0U, failed = 0U;
            for (size_t d = 0U; d < task->dependency_count; ++d) {
                const size_t index = find_index(graph, task->dependencies[d]);
                if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
                if (is_success_like(states[index].state) ||
                    (is_failure_like(states[index].state) && dependency_allows_failure(&graph->nodes[index])))
                    satisfied += 1U;
                else if (is_failure_like(states[index].state)) failed += 1U;
            }
            const UmiBuildTaskState next = failed > 0U ? UMI_BUILD_TASK_BLOCKED :
                satisfied == task->dependency_count ? UMI_BUILD_TASK_READY : UMI_BUILD_TASK_PENDING;
            if (next != node->state || satisfied != node->satisfied || failed != node->failed) {
                UmiStatus status = runtime_advance(node);
                if (status != UMI_STATUS_OK) return status;
                changed = next != node->state || changed;
                node->state = next; node->satisfied = satisfied; node->failed = failed;
            }
        }
    } while (changed);
    return UMI_STATUS_OK;
}

static UmiStatus runtime_publish(UmiBuildTaskGraph *graph, const TaskGraphRuntime *states,
                                uint64_t revision_steps)
{
    if (graph->revision > UINT64_MAX - revision_steps) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < graph->count; ++i) {
        UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[i];
        node->state = states[i].state;
        node->satisfied_dependency_count = states[i].satisfied;
        node->failed_dependency_count = states[i].failed;
        node->progress_basis_points = states[i].progress;
        node->attempt_count = states[i].attempts;
        node->last_status = states[i].status;
        node->last_exit_code = states[i].exit_code;
        node->revision = states[i].revision;
    }
    graph->revision += revision_steps;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_graph_create(UmiBuildTaskGraph **out_graph)
{
    UmiBuildTaskGraph *graph;
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = NULL;
    graph = (UmiBuildTaskGraph *)calloc(1U, sizeof(*graph));
    if (graph == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    graph->revision = 1U;
    *out_graph = graph;
    return UMI_STATUS_OK;
}

void umi_build_task_graph_destroy(UmiBuildTaskGraph *graph)
{
    free(graph);
}

/* Validate and refresh a complete candidate before replacing an existing graph, so missing dependencies, cycles and allocation failure preserve previous work. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_rebuild(
    UmiBuildTaskGraph *graph,
    const UmiBuildTaskRegistry *registry)
{
    size_t count;
    size_t index;
    UmiStatus status;
    if (graph == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = umi_build_task_registry_count(registry);
    if (count > UMI_BUILD_TASK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(graph->nodes, 0, sizeof(graph->nodes));
    graph->count = 0U;
    graph->dependency_count = 0U;
    graph->cyclic = 0;
    for (index = 0U; index < count; ++index) {
        UmiBuildTaskSnapshot task;
        UmiBuildTaskGraphNodeSnapshot *node;
        status = umi_build_task_registry_at(registry, index, &task);
        if (status != UMI_STATUS_OK) return status;
        node = &graph->nodes[graph->count++];
        (void)memset(node, 0, sizeof(*node));
        node->structure_size = (uint32_t)sizeof(*node);
        node->api_version = UMI_BUILD_TASK_GRAPH_API_VERSION;
        node->task = task;
        node->state = (task.flags & UMI_BUILD_TASK_ENABLED) != 0U
                          ? UMI_BUILD_TASK_PENDING
                          : UMI_BUILD_TASK_SKIPPED;
        node->last_status = UMI_STATUS_OK;
        node->revision = 1U;
        graph->dependency_count += task.dependency_count;
    }
    graph->source_revision = umi_build_task_registry_revision(registry);
    status = detect_cycle(graph);
    graph->revision += 1U;
    if (status != UMI_STATUS_OK) return status;
    return umi_build_task_graph_refresh(graph);
}
#endif

UmiStatus umi_build_task_graph_rebuild(
    UmiBuildTaskGraph *graph, const UmiBuildTaskRegistry *registry)
{
    if (graph == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (graph->revision > UINT64_MAX - 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t count = umi_build_task_registry_count(registry);
    if (count > UMI_BUILD_TASK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Rebuilding replaces task values as well as runtime state. Allocate the
     * candidate on the heap rather than placing large task arrays on a stack. */
    UmiBuildTaskGraph *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    candidate->revision = graph->revision + 1U;
    candidate->source_revision = umi_build_task_registry_revision(registry);
    for (size_t i = 0U; i < count; ++i) {
        UmiBuildTaskGraphNodeSnapshot *node = &candidate->nodes[i];
        status = umi_build_task_registry_at(registry, i, &node->task);
        if (status != UMI_STATUS_OK) break;
        node->structure_size = (uint32_t)sizeof(*node);
        node->api_version = UMI_BUILD_TASK_GRAPH_API_VERSION;
        node->state = (node->task.flags & UMI_BUILD_TASK_ENABLED) != 0U ? UMI_BUILD_TASK_PENDING : UMI_BUILD_TASK_SKIPPED;
        node->last_status = UMI_STATUS_OK;
        node->revision = 1U;
        candidate->dependency_count += node->task.dependency_count;
        candidate->count += 1U;
    }
    if (status == UMI_STATUS_OK) status = detect_cycle(candidate);
    if (status == UMI_STATUS_OK) status = umi_build_task_graph_refresh(candidate);
    if (status == UMI_STATUS_OK) *graph = *candidate;
    free(candidate);
    return status;
}

/* Stage reset and its readiness refresh together so exhausted downstream identities cannot leave a partially reset graph. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_reset(UmiBuildTaskGraph *graph)
{
    size_t index;
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < graph->count; ++index) {
        UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[index];
        node->state = (node->task.flags & UMI_BUILD_TASK_ENABLED) != 0U
                          ? UMI_BUILD_TASK_PENDING
                          : UMI_BUILD_TASK_SKIPPED;
        node->satisfied_dependency_count = 0U;
        node->failed_dependency_count = 0U;
        node->progress_basis_points = 0U;
        node->attempt_count = 0U;
        node->last_status = UMI_STATUS_OK;
        node->last_exit_code = 0;
        node->revision += 1U;
    }
    graph->revision += 1U;
    return umi_build_task_graph_refresh(graph);
}
#endif

UmiStatus umi_build_task_graph_reset(UmiBuildTaskGraph *graph)
{
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    runtime_capture(graph, states);
    for (size_t i = 0U; i < graph->count; ++i) {
        UmiStatus status = runtime_advance(&states[i]);
        if (status != UMI_STATUS_OK) return status;
        states[i].state = (graph->nodes[i].task.flags & UMI_BUILD_TASK_ENABLED) != 0U ? UMI_BUILD_TASK_PENDING : UMI_BUILD_TASK_SKIPPED;
        states[i].satisfied = 0U; states[i].failed = 0U; states[i].progress = 0U;
        states[i].attempts = 0U; states[i].status = UMI_STATUS_OK; states[i].exit_code = 0;
    }
    UmiStatus status = runtime_refresh(graph, states);
    return status == UMI_STATUS_OK ? runtime_publish(graph, states, 2U) : status;
}

/* Plan all readiness and dependency-counter changes before publication and advance identities for changed observable counts as well as states. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_refresh(UmiBuildTaskGraph *graph)
{
    size_t index;
    int changed;
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (graph->cyclic) return UMI_STATUS_INVALID_STATE;
    do {
        changed = 0;
        for (index = 0U; index < graph->count; ++index) {
            UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[index];
            size_t dependency;
            size_t satisfied = 0U;
            size_t failed = 0U;
            UmiBuildTaskState new_state;
            if (node->state == UMI_BUILD_TASK_RUNNING ||
                node->state == UMI_BUILD_TASK_SUCCEEDED ||
                node->state == UMI_BUILD_TASK_FAILED ||
                node->state == UMI_BUILD_TASK_CANCELLED ||
                node->state == UMI_BUILD_TASK_SKIPPED) {
                continue;
            }
            for (dependency = 0U;
                 dependency < node->task.dependency_count;
                 ++dependency) {
                const size_t dependency_index = find_index(
                    graph, node->task.dependencies[dependency]);
                const UmiBuildTaskGraphNodeSnapshot *dependency_node;
                if (dependency_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
                dependency_node = &graph->nodes[dependency_index];
                if (is_success_like(dependency_node->state) ||
                    (is_failure_like(dependency_node->state) &&
                     dependency_allows_failure(dependency_node))) {
                    satisfied += 1U;
                } else if (is_failure_like(dependency_node->state)) {
                    failed += 1U;
                }
            }
            node->satisfied_dependency_count = satisfied;
            node->failed_dependency_count = failed;
            if (failed > 0U) new_state = UMI_BUILD_TASK_BLOCKED;
            else if (satisfied == node->task.dependency_count)
                new_state = UMI_BUILD_TASK_READY;
            else
                new_state = UMI_BUILD_TASK_PENDING;
            if (new_state != node->state) {
                node->state = new_state;
                node->revision += 1U;
                changed = 1;
            }
        }
    } while (changed != 0);
    graph->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_task_graph_refresh(UmiBuildTaskGraph *graph)
{
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    runtime_capture(graph, states);
    UmiStatus status = runtime_refresh(graph, states);
    return status == UMI_STATUS_OK ? runtime_publish(graph, states, 1U) : status;
}

UmiStatus umi_build_task_graph_find(
    const UmiBuildTaskGraph *graph,
    const char *task_id,
    UmiBuildTaskGraphNodeSnapshot *out_node)
{
    size_t index;
    if (graph == NULL || task_id == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_node = graph->nodes[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_graph_at(
    const UmiBuildTaskGraph *graph,
    size_t position,
    UmiBuildTaskGraphNodeSnapshot *out_node)
{
    if (graph == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= graph->count) return UMI_STATUS_NOT_FOUND;
    *out_node = graph->nodes[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_graph_next_ready(
    const UmiBuildTaskGraph *graph,
    UmiBuildTaskGraphNodeSnapshot *out_node)
{
    size_t index;
    if (graph == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < graph->count; ++index) {
        if (graph->nodes[index].state == UMI_BUILD_TASK_READY) {
            *out_node = graph->nodes[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_start(
    UmiBuildTaskGraph *graph,
    const char *task_id)
{
    size_t index;
    UmiBuildTaskGraphNodeSnapshot *node;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if (node->state != UMI_BUILD_TASK_READY) return UMI_STATUS_INVALID_STATE;
    if (node->attempt_count >= node->task.maximum_attempts) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    node->state = UMI_BUILD_TASK_RUNNING;
    node->attempt_count += 1U;
    node->progress_basis_points = 0U;
    node->revision += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_task_graph_start(
    UmiBuildTaskGraph *graph,
    const char *task_id)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    TaskGraphRuntime *node;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &states[index];
    if (node->state != UMI_BUILD_TASK_READY) return UMI_STATUS_INVALID_STATE;
    if (node->attempts >= graph->nodes[index].task.maximum_attempts) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    node->state = UMI_BUILD_TASK_RUNNING;
    node->attempts += 1U;
    node->progress = 0U;
    if (runtime_advance(node) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return runtime_publish(graph, states, 1U);
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_set_progress(
    UmiBuildTaskGraph *graph,
    const char *task_id,
    uint32_t progress_basis_points)
{
    size_t index;
    if (graph == NULL || task_id == NULL || progress_basis_points > 10000U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state != UMI_BUILD_TASK_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    graph->nodes[index].progress_basis_points = progress_basis_points;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_task_graph_set_progress(
    UmiBuildTaskGraph *graph,
    const char *task_id,
    uint32_t progress_basis_points)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    if (graph == NULL || task_id == NULL || progress_basis_points > 10000U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (states[index].state != UMI_BUILD_TASK_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    states[index].progress = progress_basis_points;
    if (runtime_advance(&states[index]) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return runtime_publish(graph, states, 1U);
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
static UmiStatus finish_node(UmiBuildTaskGraph *graph,
                             const char *task_id,
                             UmiBuildTaskState state,
                             UmiStatus status,
                             int exit_code)
{
    size_t index;
    UmiStatus refresh_status;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state != UMI_BUILD_TASK_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    graph->nodes[index].state = state;
    graph->nodes[index].progress_basis_points = 10000U;
    graph->nodes[index].last_status = status;
    graph->nodes[index].last_exit_code = exit_code;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    refresh_status = umi_build_task_graph_refresh(graph);
    return refresh_status;
}
#endif

static UmiStatus finish_node(UmiBuildTaskGraph *graph,
                             const char *task_id,
                             UmiBuildTaskState state,
                             UmiStatus status,
                             int exit_code)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    UmiStatus refresh_status;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (states[index].state != UMI_BUILD_TASK_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    states[index].state = state;
    states[index].progress = 10000U;
    states[index].status = status;
    states[index].exit_code = exit_code;
    if (runtime_advance(&states[index]) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    refresh_status = runtime_refresh(graph, states);
    return refresh_status == UMI_STATUS_OK ? runtime_publish(graph, states, 2U) : refresh_status;
}

UmiStatus umi_build_task_graph_complete(UmiBuildTaskGraph *graph,
                                        const char *task_id,
                                        int exit_code)
{
    if (exit_code != 0) {
        return umi_build_task_graph_fail(
            graph, task_id, UMI_STATUS_INTERNAL_ERROR, exit_code);
    }
    return finish_node(graph, task_id, UMI_BUILD_TASK_SUCCEEDED,
                       UMI_STATUS_OK, 0);
}

UmiStatus umi_build_task_graph_fail(UmiBuildTaskGraph *graph,
                                    const char *task_id,
                                    UmiStatus status,
                                    int exit_code)
{
    if (status == UMI_STATUS_OK) status = UMI_STATUS_INTERNAL_ERROR;
    return finish_node(graph, task_id, UMI_BUILD_TASK_FAILED,
                       status, exit_code);
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_cancel(UmiBuildTaskGraph *graph,
                                      const char *task_id)
{
    size_t index;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state == UMI_BUILD_TASK_SUCCEEDED ||
        graph->nodes[index].state == UMI_BUILD_TASK_FAILED ||
        graph->nodes[index].state == UMI_BUILD_TASK_SKIPPED) {
        return UMI_STATUS_INVALID_STATE;
    }
    graph->nodes[index].state = UMI_BUILD_TASK_CANCELLED;
    graph->nodes[index].progress_basis_points = 10000U;
    graph->nodes[index].last_status = UMI_STATUS_CANCELLED;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    return umi_build_task_graph_refresh(graph);
}
#endif

UmiStatus umi_build_task_graph_cancel(UmiBuildTaskGraph *graph,
                                      const char *task_id)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (states[index].state == UMI_BUILD_TASK_SUCCEEDED ||
        states[index].state == UMI_BUILD_TASK_FAILED ||
        states[index].state == UMI_BUILD_TASK_SKIPPED) {
        return UMI_STATUS_INVALID_STATE;
    }
    states[index].state = UMI_BUILD_TASK_CANCELLED;
    states[index].progress = 10000U;
    states[index].status = UMI_STATUS_CANCELLED;
    if (runtime_advance(&states[index]) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus refresh_status = runtime_refresh(graph, states);
    return refresh_status == UMI_STATUS_OK ? runtime_publish(graph, states, 2U) : refresh_status;
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_retry(UmiBuildTaskGraph *graph,
                                     const char *task_id)
{
    size_t index;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state != UMI_BUILD_TASK_FAILED &&
        graph->nodes[index].state != UMI_BUILD_TASK_CANCELLED &&
        graph->nodes[index].state != UMI_BUILD_TASK_BLOCKED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (graph->nodes[index].attempt_count >=
        graph->nodes[index].task.maximum_attempts) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->nodes[index].state = UMI_BUILD_TASK_PENDING;
    graph->nodes[index].progress_basis_points = 0U;
    graph->nodes[index].last_status = UMI_STATUS_OK;
    graph->nodes[index].last_exit_code = 0;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    return umi_build_task_graph_refresh(graph);
}
#endif

UmiStatus umi_build_task_graph_retry(UmiBuildTaskGraph *graph,
                                     const char *task_id)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (states[index].state != UMI_BUILD_TASK_FAILED &&
        states[index].state != UMI_BUILD_TASK_CANCELLED &&
        states[index].state != UMI_BUILD_TASK_BLOCKED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (states[index].attempts >=
        graph->nodes[index].task.maximum_attempts) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    states[index].state = UMI_BUILD_TASK_PENDING;
    states[index].progress = 0U;
    states[index].status = UMI_STATUS_OK;
    states[index].exit_code = 0;
    if (runtime_advance(&states[index]) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus refresh_status = runtime_refresh(graph, states);
    return refresh_status == UMI_STATUS_OK ? runtime_publish(graph, states, 2U) : refresh_status;
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_skip(UmiBuildTaskGraph *graph,
                                    const char *task_id)
{
    size_t index;
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state == UMI_BUILD_TASK_RUNNING ||
        graph->nodes[index].state == UMI_BUILD_TASK_SUCCEEDED) {
        return UMI_STATUS_INVALID_STATE;
    }
    graph->nodes[index].state = UMI_BUILD_TASK_SKIPPED;
    graph->nodes[index].progress_basis_points = 10000U;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    return umi_build_task_graph_refresh(graph);
}
#endif

UmiStatus umi_build_task_graph_skip(UmiBuildTaskGraph *graph,
                                    const char *task_id)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    if (graph == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    index = find_index(graph, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (states[index].state == UMI_BUILD_TASK_RUNNING ||
        states[index].state == UMI_BUILD_TASK_SUCCEEDED) {
        return UMI_STATUS_INVALID_STATE;
    }
    states[index].state = UMI_BUILD_TASK_SKIPPED;
    states[index].progress = 10000U;
    if (runtime_advance(&states[index]) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus refresh_status = runtime_refresh(graph, states);
    return refresh_status == UMI_STATUS_OK ? runtime_publish(graph, states, 2U) : refresh_status;
}

/* Stage runtime transitions and every affected identity before publishing; preserve existing lifecycle and dependency policy without copying task records per operation. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_task_graph_cancel_waiting(UmiBuildTaskGraph *graph)
{
    size_t index;
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < graph->count; ++index) {
        UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[index];
        if (node->state == UMI_BUILD_TASK_PENDING ||
            node->state == UMI_BUILD_TASK_READY ||
            node->state == UMI_BUILD_TASK_BLOCKED) {
            node->state = UMI_BUILD_TASK_CANCELLED;
            node->progress_basis_points = 10000U;
            node->last_status = UMI_STATUS_CANCELLED;
            node->revision += 1U;
        }
    }
    graph->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_task_graph_cancel_waiting(UmiBuildTaskGraph *graph)
{
    size_t index;
    TaskGraphRuntime states[UMI_BUILD_TASK_CAPACITY];
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime_capture(graph, states);
    for (index = 0U; index < graph->count; ++index) {
        TaskGraphRuntime *node = &states[index];
        if (node->state == UMI_BUILD_TASK_PENDING ||
            node->state == UMI_BUILD_TASK_READY ||
            node->state == UMI_BUILD_TASK_BLOCKED) {
            node->state = UMI_BUILD_TASK_CANCELLED;
            node->progress = 10000U;
            node->status = UMI_STATUS_CANCELLED;
            if (runtime_advance(node) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return runtime_publish(graph, states, 1U);
}

UmiStatus umi_build_task_graph_snapshot(
    const UmiBuildTaskGraph *graph,
    UmiBuildTaskGraphSnapshot *out_snapshot)
{
    size_t index;
    uint64_t progress_total = 0U;
    if (graph == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_TASK_GRAPH_API_VERSION;
    out_snapshot->task_count = graph->count;
    out_snapshot->dependency_count = graph->dependency_count;
    out_snapshot->source_revision = graph->source_revision;
    out_snapshot->revision = graph->revision;
    out_snapshot->cyclic = graph->cyclic;
    for (index = 0U; index < graph->count; ++index) {
        const UmiBuildTaskGraphNodeSnapshot *node = &graph->nodes[index];
        switch (node->state) {
        case UMI_BUILD_TASK_PENDING: out_snapshot->pending_count += 1U; break;
        case UMI_BUILD_TASK_READY: out_snapshot->ready_count += 1U; break;
        case UMI_BUILD_TASK_RUNNING: out_snapshot->running_count += 1U; break;
        case UMI_BUILD_TASK_SUCCEEDED: out_snapshot->succeeded_count += 1U; break;
        case UMI_BUILD_TASK_FAILED: out_snapshot->failed_count += 1U; break;
        case UMI_BUILD_TASK_CANCELLED: out_snapshot->cancelled_count += 1U; break;
        case UMI_BUILD_TASK_SKIPPED: out_snapshot->skipped_count += 1U; break;
        case UMI_BUILD_TASK_BLOCKED: out_snapshot->blocked_count += 1U; break;
        default: break;
        }
        progress_total += node->progress_basis_points;
    }
    if (graph->count > 0U) {
        out_snapshot->progress_basis_points =
            (uint32_t)(progress_total / graph->count);
    }
    return UMI_STATUS_OK;
}

size_t umi_build_task_graph_count(const UmiBuildTaskGraph *graph)
{
    return graph != NULL ? graph->count : 0U;
}

uint64_t umi_build_task_graph_revision(const UmiBuildTaskGraph *graph)
{
    return graph != NULL ? graph->revision : 0U;
}

const char *umi_build_task_state_text(UmiBuildTaskState state)
{
    switch (state) {
    case UMI_BUILD_TASK_PENDING: return "Pending";
    case UMI_BUILD_TASK_READY: return "Ready";
    case UMI_BUILD_TASK_RUNNING: return "Running";
    case UMI_BUILD_TASK_SUCCEEDED: return "Succeeded";
    case UMI_BUILD_TASK_FAILED: return "Failed";
    case UMI_BUILD_TASK_CANCELLED: return "Cancelled";
    case UMI_BUILD_TASK_SKIPPED: return "Skipped";
    case UMI_BUILD_TASK_BLOCKED: return "Blocked";
    default: return "Unknown";
    }
}
