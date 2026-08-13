/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/graph.c
 *
 * PURPOSE:
 *   Implement a deterministic, bounded build DAG with explicit progress,
 *   incremental invalidation, cancellation, timeout and retry transitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/graph.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiBuildGraphDependency {
    size_t node_index;
    size_t dependency_index;
} UmiBuildGraphDependency;

struct UmiBuildGraph {
    UmiBuildGraphNodeSnapshot nodes[UMI_BUILD_GRAPH_MAX_NODES];
    UmiBuildGraphDependency dependencies[UMI_BUILD_GRAPH_MAX_DEPENDENCIES];
    size_t node_count;
    size_t dependency_count;
    uint64_t revision;
};

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

static size_t find_index(const UmiBuildGraph *graph, const char *node_id)
{
    size_t index;
    if (graph == NULL || node_id == NULL) return SIZE_MAX;
    for (index = 0U; index < graph->node_count; ++index) {
        if (strcmp(graph->nodes[index].node_id, node_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int dependency_exists(const UmiBuildGraph *graph,
                             size_t node_index,
                             size_t dependency_index)
{
    size_t index;
    for (index = 0U; index < graph->dependency_count; ++index) {
        if (graph->dependencies[index].node_index == node_index &&
            graph->dependencies[index].dependency_index == dependency_index) {
            return 1;
        }
    }
    return 0;
}

static int transitively_depends_on(const UmiBuildGraph *graph,
                                   size_t start_index,
                                   size_t target_index)
{
    size_t stack[UMI_BUILD_GRAPH_MAX_NODES];
    unsigned char visited[UMI_BUILD_GRAPH_MAX_NODES] = {0};
    size_t count = 0U;
    stack[count++] = start_index;
    while (count > 0U) {
        size_t current = stack[--count];
        size_t index;
        if (current == target_index) return 1;
        if (visited[current] != 0U) continue;
        visited[current] = 1U;
        for (index = 0U; index < graph->dependency_count; ++index) {
            if (graph->dependencies[index].node_index == current &&
                visited[graph->dependencies[index].dependency_index] == 0U) {
                if (count >= UMI_BUILD_GRAPH_MAX_NODES) return 1;
                stack[count++] = graph->dependencies[index].dependency_index;
            }
        }
    }
    return 0;
}

static int dependency_succeeded(UmiBuildNodeState state)
{
    return state == UMI_BUILD_NODE_SUCCEEDED ||
           state == UMI_BUILD_NODE_SKIPPED;
}

static int dependency_failed(UmiBuildNodeState state)
{
    return state == UMI_BUILD_NODE_FAILED ||
           state == UMI_BUILD_NODE_CANCELLED ||
           state == UMI_BUILD_NODE_TIMED_OUT ||
           state == UMI_BUILD_NODE_BLOCKED;
}

static int all_dependencies_succeeded(const UmiBuildGraph *graph,
                                      size_t node_index)
{
    size_t index;
    for (index = 0U; index < graph->dependency_count; ++index) {
        if (graph->dependencies[index].node_index == node_index &&
            !dependency_succeeded(graph->nodes[
                graph->dependencies[index].dependency_index].state)) return 0;
    }
    return 1;
}

static int any_dependency_failed(const UmiBuildGraph *graph,
                                 size_t node_index)
{
    size_t index;
    for (index = 0U; index < graph->dependency_count; ++index) {
        if (graph->dependencies[index].node_index == node_index &&
            dependency_failed(graph->nodes[
                graph->dependencies[index].dependency_index].state)) return 1;
    }
    return 0;
}

void umi_build_graph_node_init(UmiBuildGraphNodeSnapshot *node,
                               const char *node_id,
                               const char *label,
                               UmiBuildPhase phase)
{
    if (node == NULL) return;
    memset(node, 0, sizeof(*node));
    node->structure_size = (uint32_t)sizeof(*node);
    node->api_version = UMI_BUILD_GRAPH_API_VERSION;
    copy_text(node->node_id, sizeof(node->node_id), node_id);
    copy_text(node->label, sizeof(node->label), label);
    node->phase = phase;
    node->state = UMI_BUILD_NODE_PENDING;
    node->maximum_attempts = 1U;
    node->input_revision = 1U;
    node->last_status = UMI_STATUS_OK;
    node->revision = 1U;
}

UmiStatus umi_build_graph_create(UmiBuildGraph **out_graph)
{
    UmiBuildGraph *graph;
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = NULL;
    graph = (UmiBuildGraph *)calloc(1U, sizeof(*graph));
    if (graph == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    graph->revision = 1U;
    *out_graph = graph;
    return UMI_STATUS_OK;
}

void umi_build_graph_destroy(UmiBuildGraph *graph) { free(graph); }

UmiStatus umi_build_graph_add_node(UmiBuildGraph *graph,
                                   const UmiBuildGraphNodeSnapshot *node)
{
    UmiBuildGraphNodeSnapshot copy;
    if (graph == NULL || node == NULL || node->node_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (graph->node_count >= UMI_BUILD_GRAPH_MAX_NODES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (find_index(graph, node->node_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    copy = *node;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_GRAPH_API_VERSION;
    copy.node_id[UMI_BUILD_ID_CAPACITY - 1U] = '\0';
    copy.label[UMI_BUILD_NAME_CAPACITY - 1U] = '\0';
    copy.state = UMI_BUILD_NODE_PENDING;
    copy.progress_basis_points = 0U;
    copy.attempt_count = 0U;
    if (copy.maximum_attempts == 0U) copy.maximum_attempts = 1U;
    if (copy.input_revision == 0U) copy.input_revision = 1U;
    copy.completed_input_revision = 0U;
    copy.revision = 1U;
    graph->nodes[graph->node_count++] = copy;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_add_dependency(UmiBuildGraph *graph,
                                         const char *node_id,
                                         const char *depends_on_node_id)
{
    size_t node_index;
    size_t dependency_index;
    if (graph == NULL || node_id == NULL || depends_on_node_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    node_index = find_index(graph, node_id);
    dependency_index = find_index(graph, depends_on_node_id);
    if (node_index == SIZE_MAX || dependency_index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    if (node_index == dependency_index) return UMI_STATUS_INVALID_ARGUMENT;
    if (dependency_exists(graph, node_index, dependency_index))
        return UMI_STATUS_ALREADY_EXISTS;
    if (graph->dependency_count >= UMI_BUILD_GRAPH_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (transitively_depends_on(graph, dependency_index, node_index))
        return UMI_STATUS_INVALID_STATE;
    graph->dependencies[graph->dependency_count].node_index = node_index;
    graph->dependencies[graph->dependency_count].dependency_index =
        dependency_index;
    graph->dependency_count += 1U;
    graph->nodes[node_index].state = UMI_BUILD_NODE_PENDING;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_find(const UmiBuildGraph *graph,
                               const char *node_id,
                               UmiBuildGraphNodeSnapshot *out_node)
{
    size_t index;
    if (graph == NULL || node_id == NULL || out_node == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_node = graph->nodes[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_graph_at(const UmiBuildGraph *graph,
                             size_t index,
                             UmiBuildGraphNodeSnapshot *out_node)
{
    if (graph == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= graph->node_count) return UMI_STATUS_NOT_FOUND;
    *out_node = graph->nodes[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_graph_refresh(UmiBuildGraph *graph)
{
    int any_changed = 0;
    int pass_changed;
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Iterate to a fixed point.  Nodes may be registered in any order, so a
     * single pass cannot reliably propagate a failure through a deep graph. */
    do {
        size_t index;
        pass_changed = 0;
        for (index = 0U; index < graph->node_count; ++index) {
            UmiBuildGraphNodeSnapshot *node = &graph->nodes[index];
            UmiBuildNodeState next = node->state;
            if (node->state == UMI_BUILD_NODE_PENDING ||
                node->state == UMI_BUILD_NODE_READY ||
                node->state == UMI_BUILD_NODE_BLOCKED) {
                if (any_dependency_failed(graph, index)) {
                    next = UMI_BUILD_NODE_BLOCKED;
                } else if (all_dependencies_succeeded(graph, index)) {
                    next = UMI_BUILD_NODE_READY;
                } else {
                    next = UMI_BUILD_NODE_PENDING;
                }
            }
            if (next != node->state) {
                node->state = next;
                node->revision += 1U;
                pass_changed = 1;
                any_changed = 1;
            }
        }
    } while (pass_changed != 0);
    if (any_changed != 0) graph->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_graph_next_ready(UmiBuildGraph *graph,
                                     UmiBuildGraphNodeSnapshot *out_node)
{
    size_t index;
    if (graph == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_build_graph_refresh(graph);
    for (index = 0U; index < graph->node_count; ++index) {
        if (graph->nodes[index].state == UMI_BUILD_NODE_READY) {
            *out_node = graph->nodes[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_build_graph_start(UmiBuildGraph *graph, const char *node_id)
{
    size_t index;
    UmiBuildGraphNodeSnapshot *node;
    if (graph == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_build_graph_refresh(graph);
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if (node->state != UMI_BUILD_NODE_READY ||
        node->attempt_count >= node->maximum_attempts)
        return UMI_STATUS_INVALID_STATE;
    node->state = UMI_BUILD_NODE_RUNNING;
    node->attempt_count += 1U;
    node->progress_basis_points = 0U;
    node->revision += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_graph_set_progress(UmiBuildGraph *graph,
                                       const char *node_id,
                                       uint32_t progress_basis_points)
{
    size_t index;
    if (graph == NULL || node_id == NULL || progress_basis_points > 10000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (graph->nodes[index].state != UMI_BUILD_NODE_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    graph->nodes[index].progress_basis_points = progress_basis_points;
    graph->nodes[index].revision += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus finish_node(UmiBuildGraph *graph,
                             const char *node_id,
                             UmiBuildNodeState state,
                             UmiStatus status,
                             int exit_code)
{
    size_t index;
    UmiBuildGraphNodeSnapshot *node;
    if (graph == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if (node->state != UMI_BUILD_NODE_RUNNING) return UMI_STATUS_INVALID_STATE;
    node->state = state;
    node->last_status = status;
    node->last_exit_code = exit_code;
    if (state == UMI_BUILD_NODE_SUCCEEDED) {
        node->progress_basis_points = 10000U;
        node->completed_input_revision = node->input_revision;
    }
    node->revision += 1U;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_complete(UmiBuildGraph *graph,
                                   const char *node_id,
                                   int exit_code)
{
    return finish_node(graph, node_id, UMI_BUILD_NODE_SUCCEEDED,
                       UMI_STATUS_OK, exit_code);
}

UmiStatus umi_build_graph_fail(UmiBuildGraph *graph,
                               const char *node_id,
                               UmiStatus status,
                               int exit_code)
{
    return finish_node(graph, node_id, UMI_BUILD_NODE_FAILED,
                       status, exit_code);
}

UmiStatus umi_build_graph_cancel(UmiBuildGraph *graph, const char *node_id)
{
    return finish_node(graph, node_id, UMI_BUILD_NODE_CANCELLED,
                       UMI_STATUS_CANCELLED, -1);
}

UmiStatus umi_build_graph_timeout(UmiBuildGraph *graph, const char *node_id)
{
    return finish_node(graph, node_id, UMI_BUILD_NODE_TIMED_OUT,
                       UMI_STATUS_TIMEOUT, -1);
}

UmiStatus umi_build_graph_retry(UmiBuildGraph *graph, const char *node_id)
{
    size_t index;
    UmiBuildGraphNodeSnapshot *node;
    if (graph == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if ((node->state != UMI_BUILD_NODE_FAILED &&
         node->state != UMI_BUILD_NODE_CANCELLED &&
         node->state != UMI_BUILD_NODE_TIMED_OUT) ||
        node->attempt_count >= node->maximum_attempts)
        return UMI_STATUS_INVALID_STATE;
    node->state = UMI_BUILD_NODE_PENDING;
    node->progress_basis_points = 0U;
    node->last_status = UMI_STATUS_OK;
    node->last_exit_code = 0;
    node->revision += 1U;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_invalidate(UmiBuildGraph *graph,
                                     const char *node_id,
                                     uint64_t input_revision)
{
    size_t index;
    UmiBuildGraphNodeSnapshot *node;
    if (graph == NULL || node_id == NULL || input_revision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if (node->state == UMI_BUILD_NODE_RUNNING) return UMI_STATUS_BUSY;
    node->input_revision = input_revision;
    /* Re-queue even when the revision is unchanged.  The engine then records
     * an explicit SKIPPED result instead of silently leaving a stale success. */
    node->state = UMI_BUILD_NODE_PENDING;
    node->progress_basis_points = 0U;
    node->attempt_count = 0U;
    node->revision += 1U;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_skip_unchanged(UmiBuildGraph *graph,
                                         const char *node_id)
{
    size_t index;
    UmiBuildGraphNodeSnapshot *node;
    if (graph == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(graph, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    node = &graph->nodes[index];
    if (!node->incremental || node->state != UMI_BUILD_NODE_READY ||
        node->completed_input_revision == 0U ||
        node->completed_input_revision != node->input_revision)
        return UMI_STATUS_INVALID_STATE;
    node->state = UMI_BUILD_NODE_SKIPPED;
    node->progress_basis_points = 10000U;
    node->revision += 1U;
    graph->revision += 1U;
    return umi_build_graph_refresh(graph);
}

UmiStatus umi_build_graph_snapshot(UmiBuildGraph *graph,
                                   UmiBuildGraphSnapshot *out_snapshot)
{
    size_t index;
    uint64_t progress = 0U;
    if (graph == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_build_graph_refresh(graph);
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_GRAPH_API_VERSION;
    out_snapshot->node_count = graph->node_count;
    out_snapshot->dependency_count = graph->dependency_count;
    out_snapshot->revision = graph->revision;
    for (index = 0U; index < graph->node_count; ++index) {
        UmiBuildNodeState state = graph->nodes[index].state;
        progress += graph->nodes[index].progress_basis_points;
        switch (state) {
            case UMI_BUILD_NODE_PENDING: out_snapshot->pending_count++; break;
            case UMI_BUILD_NODE_READY: out_snapshot->ready_count++; break;
            case UMI_BUILD_NODE_RUNNING: out_snapshot->running_count++; break;
            case UMI_BUILD_NODE_SUCCEEDED: out_snapshot->succeeded_count++; break;
            case UMI_BUILD_NODE_FAILED: out_snapshot->failed_count++; break;
            case UMI_BUILD_NODE_CANCELLED: out_snapshot->cancelled_count++; break;
            case UMI_BUILD_NODE_TIMED_OUT: out_snapshot->timed_out_count++; break;
            case UMI_BUILD_NODE_SKIPPED: out_snapshot->skipped_count++; break;
            case UMI_BUILD_NODE_BLOCKED: out_snapshot->blocked_count++; break;
            default: return UMI_STATUS_INTERNAL_ERROR;
        }
    }
    out_snapshot->progress_basis_points = graph->node_count == 0U
        ? 0U : (uint32_t)(progress / graph->node_count);
    return UMI_STATUS_OK;
}
