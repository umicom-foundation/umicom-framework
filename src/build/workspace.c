/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/workspace.c
 *
 * PURPOSE:
 *   Implement one reusable build-and-task workspace over existing Framework
 *   graph, history, artifact and task services without duplicating execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/workspace.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiBuildWorkspace {
    UmiBuildGraph *graph;
    UmiBuildHistory *history;
    UmiBuildArtifactIndex *artifacts;
    UmiTaskQueue *task_queue;
    const UmiBuildProfile *profile;
    char filter_text[UMI_BUILD_WORKSPACE_FILTER_CAPACITY];
    UmiBuildWorkspaceNodeFilter node_filter;
    char selected_node_id[UMI_BUILD_ID_CAPACITY];
    uint64_t selected_operation_id;
    char selected_artifact_id[UMI_BUILD_ID_CAPACITY];
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

static int valid_filter(UmiBuildWorkspaceNodeFilter node_filter)
{
    return node_filter >= UMI_BUILD_WORKSPACE_NODES_ALL &&
           node_filter <= UMI_BUILD_WORKSPACE_NODES_BLOCKED;
}

static int text_contains_case_insensitive(const char *text, const char *query)
{
    const unsigned char *candidate;
    size_t query_length;

    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    query_length = strlen(query);
    for (candidate = (const unsigned char *)text;
         *candidate != '\0'; ++candidate) {
        size_t index;
        for (index = 0U; index < query_length; ++index) {
            unsigned char left = candidate[index];
            unsigned char right = (unsigned char)query[index];
            if (left == '\0' || tolower(left) != tolower(right)) break;
        }
        if (index == query_length) return 1;
    }
    return 0;
}

static int node_state_matches(UmiBuildNodeState state,
                              UmiBuildWorkspaceNodeFilter node_filter)
{
    switch (node_filter) {
        case UMI_BUILD_WORKSPACE_NODES_PENDING:
            return state == UMI_BUILD_NODE_PENDING;
        case UMI_BUILD_WORKSPACE_NODES_READY:
            return state == UMI_BUILD_NODE_READY;
        case UMI_BUILD_WORKSPACE_NODES_RUNNING:
            return state == UMI_BUILD_NODE_RUNNING;
        case UMI_BUILD_WORKSPACE_NODES_SUCCEEDED:
            return state == UMI_BUILD_NODE_SUCCEEDED ||
                   state == UMI_BUILD_NODE_SKIPPED;
        case UMI_BUILD_WORKSPACE_NODES_FAILED:
            return state == UMI_BUILD_NODE_FAILED ||
                   state == UMI_BUILD_NODE_CANCELLED ||
                   state == UMI_BUILD_NODE_TIMED_OUT;
        case UMI_BUILD_WORKSPACE_NODES_BLOCKED:
            return state == UMI_BUILD_NODE_BLOCKED;
        case UMI_BUILD_WORKSPACE_NODES_ALL:
        default:
            return 1;
    }
}

static int node_matches(const UmiBuildWorkspace *workspace,
                        const UmiBuildGraphNodeSnapshot *node)
{
    const char *phase;
    const char *state;

    if (!node_state_matches(node->state, workspace->node_filter)) return 0;
    if (workspace->filter_text[0] == '\0') return 1;
    phase = umi_build_phase_text(node->phase);
    state = umi_build_node_state_text(node->state);
    return text_contains_case_insensitive(node->node_id,
                                          workspace->filter_text) ||
           text_contains_case_insensitive(node->label,
                                          workspace->filter_text) ||
           text_contains_case_insensitive(phase, workspace->filter_text) ||
           text_contains_case_insensitive(state, workspace->filter_text);
}

static UmiStatus find_history_operation(const UmiBuildWorkspace *workspace,
                                        uint64_t operation_id,
                                        UmiBuildResult *out_result)
{
    UmiBuildResult *result = NULL;
    size_t index;
    size_t count;
    UmiStatus status;

    if (operation_id == 0U) return UMI_STATUS_NOT_FOUND;
    status = umi_build_result_create(&result);
    if (status != UMI_STATUS_OK) return status;
    count = umi_build_history_count(workspace->history);
    for (index = 0U; index < count; ++index) {
        status = umi_build_history_at(workspace->history, index, result);
        if (status != UMI_STATUS_OK) {
            umi_build_result_destroy(result);
            return status;
        }
        if (result->operation_id == operation_id) {
            if (out_result != NULL) *out_result = *result;
            umi_build_result_destroy(result);
            return UMI_STATUS_OK;
        }
    }
    umi_build_result_destroy(result);
    return UMI_STATUS_NOT_FOUND;
}

static size_t visible_node_count(UmiBuildWorkspace *workspace)
{
    UmiBuildGraphSnapshot graph_snapshot;
    size_t graph_index;
    size_t count = 0U;

    if (umi_build_graph_snapshot(workspace->graph, &graph_snapshot) !=
        UMI_STATUS_OK) return 0U;
    for (graph_index = 0U; graph_index < graph_snapshot.node_count;
         ++graph_index) {
        UmiBuildGraphNodeSnapshot node;
        if (umi_build_graph_at(workspace->graph, graph_index, &node) ==
                UMI_STATUS_OK && node_matches(workspace, &node)) {
            count += 1U;
        }
    }
    return count;
}

static void reconcile_selection(UmiBuildWorkspace *workspace)
{
    UmiBuildGraphNodeSnapshot node;
    UmiBuildResult *result = NULL;
    UmiBuildArtifactSnapshot artifact;

    if (workspace->selected_node_id[0] == '\0' ||
        umi_build_graph_find(workspace->graph, workspace->selected_node_id,
                             &node) != UMI_STATUS_OK ||
        !node_matches(workspace, &node)) {
        if (umi_build_workspace_visible_node_at(workspace, 0U, &node) ==
            UMI_STATUS_OK) {
            copy_text(workspace->selected_node_id,
                      sizeof(workspace->selected_node_id), node.node_id);
        } else {
            workspace->selected_node_id[0] = '\0';
        }
    }

    if (find_history_operation(workspace, workspace->selected_operation_id,
                               NULL) != UMI_STATUS_OK) {
        if (umi_build_result_create(&result) == UMI_STATUS_OK &&
            umi_build_history_latest(workspace->history, result) ==
            UMI_STATUS_OK) {
            workspace->selected_operation_id = result->operation_id;
        } else {
            workspace->selected_operation_id = 0U;
        }
        umi_build_result_destroy(result);
    }

    if (workspace->selected_artifact_id[0] == '\0' ||
        umi_build_artifact_index_find(workspace->artifacts,
            workspace->selected_artifact_id, &artifact) != UMI_STATUS_OK) {
        if (umi_build_artifact_index_at(workspace->artifacts, 0U, &artifact) ==
            UMI_STATUS_OK) {
            copy_text(workspace->selected_artifact_id,
                      sizeof(workspace->selected_artifact_id),
                      artifact.artifact_id);
        } else {
            workspace->selected_artifact_id[0] = '\0';
        }
    }
}

void umi_build_workspace_bindings_init(UmiBuildWorkspaceBindings *bindings)
{
    if (bindings == NULL) return;
    memset(bindings, 0, sizeof(*bindings));
    bindings->structure_size = (uint32_t)sizeof(*bindings);
    bindings->api_version = UMI_BUILD_WORKSPACE_API_VERSION;
}

UmiStatus umi_build_workspace_create(
    const UmiBuildWorkspaceBindings *bindings,
    UmiBuildWorkspace **out_workspace)
{
    UmiBuildWorkspace *workspace;

    if (bindings == NULL || out_workspace == NULL ||
        bindings->structure_size < sizeof(*bindings) ||
        bindings->api_version != UMI_BUILD_WORKSPACE_API_VERSION ||
        bindings->graph == NULL || bindings->history == NULL ||
        bindings->artifacts == NULL || bindings->profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workspace = NULL;
    workspace = (UmiBuildWorkspace *)calloc(1U, sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->graph = bindings->graph;
    workspace->history = bindings->history;
    workspace->artifacts = bindings->artifacts;
    workspace->task_queue = bindings->task_queue;
    workspace->profile = bindings->profile;
    workspace->node_filter = UMI_BUILD_WORKSPACE_NODES_ALL;
    workspace->revision = 1U;
    reconcile_selection(workspace);
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

void umi_build_workspace_destroy(UmiBuildWorkspace *workspace)
{
    free(workspace);
}

UmiStatus umi_build_workspace_set_task_queue(
    UmiBuildWorkspace *workspace,
    UmiTaskQueue *task_queue)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace->task_queue = task_queue;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_set_filter(
    UmiBuildWorkspace *workspace,
    const char *text,
    UmiBuildWorkspaceNodeFilter node_filter)
{
    if (workspace == NULL || !valid_filter(node_filter))
        return UMI_STATUS_INVALID_ARGUMENT;
    copy_text(workspace->filter_text, sizeof(workspace->filter_text), text);
    workspace->node_filter = node_filter;
    workspace->revision += 1U;
    reconcile_selection(workspace);
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_select_node(
    UmiBuildWorkspace *workspace,
    const char *node_id)
{
    UmiBuildGraphNodeSnapshot node;
    UmiStatus status;

    if (workspace == NULL || node_id == NULL || node_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_graph_find(workspace->graph, node_id, &node);
    if (status != UMI_STATUS_OK) return status;
    copy_text(workspace->selected_node_id,
              sizeof(workspace->selected_node_id), node.node_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_select_operation(
    UmiBuildWorkspace *workspace,
    uint64_t operation_id)
{
    UmiStatus status;

    if (workspace == NULL || operation_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = find_history_operation(workspace, operation_id, NULL);
    if (status != UMI_STATUS_OK) return status;
    workspace->selected_operation_id = operation_id;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_select_latest_operation(
    UmiBuildWorkspace *workspace)
{
    UmiBuildResult *latest = NULL;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_result_create(&latest);
    if (status == UMI_STATUS_OK)
        status = umi_build_history_latest(workspace->history, latest);
    if (status != UMI_STATUS_OK) {
        umi_build_result_destroy(latest);
        return status;
    }
    workspace->selected_operation_id = latest->operation_id;
    umi_build_result_destroy(latest);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_select_artifact(
    UmiBuildWorkspace *workspace,
    const char *artifact_id)
{
    UmiBuildArtifactSnapshot artifact;
    UmiStatus status;

    if (workspace == NULL || artifact_id == NULL || artifact_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_artifact_index_find(workspace->artifacts, artifact_id,
                                           &artifact);
    if (status != UMI_STATUS_OK) return status;
    copy_text(workspace->selected_artifact_id,
              sizeof(workspace->selected_artifact_id), artifact.artifact_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_refresh(UmiBuildWorkspace *workspace)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_graph_refresh(workspace->graph);
    if (status == UMI_STATUS_OK) {
        reconcile_selection(workspace);
        workspace->revision += 1U;
    }
    return status;
}

void umi_build_workspace_clear_history(UmiBuildWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_build_history_clear(workspace->history);
    workspace->selected_operation_id = 0U;
    workspace->revision += 1U;
}

UmiStatus umi_build_workspace_snapshot(
    UmiBuildWorkspace *workspace,
    UmiBuildWorkspaceSnapshot *out_snapshot)
{
    UmiBuildResult *latest = NULL;
    UmiBuildGraphNodeSnapshot selected_node;
    UmiBuildArtifactSnapshot selected_artifact;
    UmiStatus status;

    if (workspace == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_build_graph_snapshot(workspace->graph, &out_snapshot->graph);
    if (status != UMI_STATUS_OK) return status;
    reconcile_selection(workspace);

    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_WORKSPACE_API_VERSION;
    out_snapshot->tasks = workspace->task_queue != NULL
        ? umi_task_queue_stats(workspace->task_queue)
        : (UmiTaskQueueStats){0};
    copy_text(out_snapshot->profile_id, sizeof(out_snapshot->profile_id),
              workspace->profile->profile_id);
    copy_text(out_snapshot->source_directory,
              sizeof(out_snapshot->source_directory),
              workspace->profile->source_directory);
    copy_text(out_snapshot->build_directory,
              sizeof(out_snapshot->build_directory),
              workspace->profile->build_directory);
    copy_text(out_snapshot->configuration,
              sizeof(out_snapshot->configuration),
              workspace->profile->configuration);
    copy_text(out_snapshot->generator, sizeof(out_snapshot->generator),
              workspace->profile->generator);
    copy_text(out_snapshot->build_target,
              sizeof(out_snapshot->build_target),
              workspace->profile->build_target);
    copy_text(out_snapshot->filter_text, sizeof(out_snapshot->filter_text),
              workspace->filter_text);
    out_snapshot->node_filter = workspace->node_filter;
    copy_text(out_snapshot->selected_node_id,
              sizeof(out_snapshot->selected_node_id),
              workspace->selected_node_id);
    out_snapshot->selected_operation_id = workspace->selected_operation_id;
    copy_text(out_snapshot->selected_artifact_id,
              sizeof(out_snapshot->selected_artifact_id),
              workspace->selected_artifact_id);
    out_snapshot->visible_node_count = visible_node_count(workspace);
    out_snapshot->history_count = umi_build_history_count(workspace->history);
    out_snapshot->artifact_count =
        umi_build_artifact_index_count(workspace->artifacts);
    out_snapshot->revision = workspace->revision +
        out_snapshot->graph.revision +
        umi_build_artifact_index_revision(workspace->artifacts) +
        (uint64_t)out_snapshot->history_count + out_snapshot->tasks.submitted +
        out_snapshot->tasks.completed;

    if (out_snapshot->history_count > 0U) {
        status = umi_build_result_create(&latest);
        if (status != UMI_STATUS_OK) return status;
        status = umi_build_history_latest(workspace->history, latest);
        if (status == UMI_STATUS_OK) {
            out_snapshot->has_latest_result = 1;
            out_snapshot->latest_phase = latest->phase;
            out_snapshot->latest_state = latest->state;
            out_snapshot->latest_status = latest->status;
            out_snapshot->latest_exit_code = latest->exit_code;
            out_snapshot->latest_duration_ms = latest->duration_ms;
            out_snapshot->latest_diagnostic_count = latest->diagnostics.count;
        }
        umi_build_result_destroy(latest);
        if (status != UMI_STATUS_OK) return status;
    }
    out_snapshot->has_selected_node =
        workspace->selected_node_id[0] != '\0' &&
        umi_build_graph_find(workspace->graph,
            workspace->selected_node_id, &selected_node) == UMI_STATUS_OK;
    out_snapshot->has_selected_operation =
        find_history_operation(workspace, workspace->selected_operation_id,
                               NULL) == UMI_STATUS_OK;
    out_snapshot->has_selected_artifact =
        workspace->selected_artifact_id[0] != '\0' &&
        umi_build_artifact_index_find(workspace->artifacts,
            workspace->selected_artifact_id, &selected_artifact) ==
            UMI_STATUS_OK;
    out_snapshot->can_execute_next = out_snapshot->graph.ready_count > 0U &&
        out_snapshot->graph.running_count == 0U;
    out_snapshot->can_execute_all = out_snapshot->can_execute_next;
    out_snapshot->can_retry_selected = out_snapshot->has_selected_node &&
        (selected_node.state == UMI_BUILD_NODE_FAILED ||
         selected_node.state == UMI_BUILD_NODE_CANCELLED ||
         selected_node.state == UMI_BUILD_NODE_TIMED_OUT ||
         selected_node.state == UMI_BUILD_NODE_BLOCKED);
    /*
     * The shared task queue is observational here.  A build command must not
     * imply that it can cancel unrelated background services, so cooperative
     * cancellation is enabled only while the bound build graph is running.
     */
    out_snapshot->can_cancel = out_snapshot->graph.running_count > 0U;
    out_snapshot->can_clear_history = out_snapshot->history_count > 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_workspace_visible_node_at(
    UmiBuildWorkspace *workspace,
    size_t index,
    UmiBuildGraphNodeSnapshot *out_node)
{
    UmiBuildGraphSnapshot graph_snapshot;
    size_t graph_index;
    size_t visible_index = 0U;
    UmiStatus status;

    if (workspace == NULL || out_node == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_graph_snapshot(workspace->graph, &graph_snapshot);
    if (status != UMI_STATUS_OK) return status;
    for (graph_index = 0U; graph_index < graph_snapshot.node_count;
         ++graph_index) {
        UmiBuildGraphNodeSnapshot node;
        status = umi_build_graph_at(workspace->graph, graph_index, &node);
        if (status != UMI_STATUS_OK) return status;
        if (!node_matches(workspace, &node)) continue;
        if (visible_index == index) {
            *out_node = node;
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_build_workspace_history_at(
    UmiBuildWorkspace *workspace,
    size_t newest_first_index,
    UmiBuildResult *out_result)
{
    size_t count;

    if (workspace == NULL || out_result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    count = umi_build_history_count(workspace->history);
    if (newest_first_index >= count) return UMI_STATUS_NOT_FOUND;
    return umi_build_history_at(workspace->history,
                                count - newest_first_index - 1U,
                                out_result);
}

UmiStatus umi_build_workspace_artifact_at(
    UmiBuildWorkspace *workspace,
    size_t index,
    UmiBuildArtifactSnapshot *out_artifact)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_build_artifact_index_at(workspace->artifacts, index,
                                       out_artifact);
}

UmiStatus umi_build_workspace_selected_result(
    UmiBuildWorkspace *workspace,
    UmiBuildResult *out_result)
{
    if (workspace == NULL || out_result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return find_history_operation(workspace, workspace->selected_operation_id,
                                  out_result);
}

UmiStatus umi_build_workspace_selected_artifact(
    UmiBuildWorkspace *workspace,
    UmiBuildArtifactSnapshot *out_artifact)
{
    if (workspace == NULL || out_artifact == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->selected_artifact_id[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    return umi_build_artifact_index_find(workspace->artifacts,
        workspace->selected_artifact_id, out_artifact);
}

const char *umi_build_workspace_node_filter_text(
    UmiBuildWorkspaceNodeFilter node_filter)
{
    switch (node_filter) {
        case UMI_BUILD_WORKSPACE_NODES_PENDING: return "pending";
        case UMI_BUILD_WORKSPACE_NODES_READY: return "ready";
        case UMI_BUILD_WORKSPACE_NODES_RUNNING: return "running";
        case UMI_BUILD_WORKSPACE_NODES_SUCCEEDED: return "succeeded";
        case UMI_BUILD_WORKSPACE_NODES_FAILED: return "failed";
        case UMI_BUILD_WORKSPACE_NODES_BLOCKED: return "blocked";
        case UMI_BUILD_WORKSPACE_NODES_ALL:
        default: return "all";
    }
}
