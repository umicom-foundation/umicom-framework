/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/task_view.c
 *
 * PURPOSE:
 *   Implement the task view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework task-queue view projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/task_view.h"
#include "view_internal.h"

#include <stdio.h>

/*
 * Initialise terminal ui task view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_ui_task_view_create(const char *view_id,
                                           UmiTerminalController *controller,
                                           UmiUiViewModel **out_view)
{
    UmiTaskQueueStats stats;
    UmiBuildGraph *graph;
    UmiBuildGraphSnapshot graph_snapshot = {0};
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_ui_base_view(view_id, "tasks", "Tasks",
        "Framework background task queue, progress and completion statistics.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    stats = umi_task_queue_stats(umi_terminal_controller_task_queue(controller));
    graph = umi_terminal_controller_operation_graph(controller);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph != NULL) {
        (void)umi_build_graph_snapshot(graph, &graph_snapshot);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.workers", (int64_t)stats.worker_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.queued", (int64_t)stats.queued);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.running", (int64_t)stats.running);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.submitted", (int64_t)stats.submitted);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.completed", (int64_t)stats.completed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.failed", (int64_t)stats.failed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "task.cancelled", (int64_t)stats.cancelled);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.count", (int64_t)graph_snapshot.node_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.ready", (int64_t)graph_snapshot.ready_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.running", (int64_t)graph_snapshot.running_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.succeeded", (int64_t)graph_snapshot.succeeded_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.failed", (int64_t)graph_snapshot.failed_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.blocked", (int64_t)graph_snapshot.blocked_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.progress-basis-points",
        (int64_t)graph_snapshot.progress_basis_points);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "operation.row-count", (int64_t)graph_snapshot.node_count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK &&
         index < graph_snapshot.node_count; ++index) {
        UmiBuildGraphNodeSnapshot node;
        char key[64];
        char value[UMI_BUILD_NAME_CAPACITY + UMI_BUILD_ID_CAPACITY + 160U];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_build_graph_at(graph, index, &node) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "operation.row.%zu", index);
        (void)snprintf(value, sizeof(value),
                       "%s  %s  %u.%02u%%  attempt %u/%u",
                       node.node_id,
                       umi_build_node_state_text(node.state),
                       node.progress_basis_points / 100U,
                       node.progress_basis_points % 100U,
                       node.attempt_count,
                       node.maximum_attempts);
        status = umi_terminal_ui_set_string(*out_view, key, value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 0U,
        "studio.action.tasks.refresh", "Refresh", "Refresh task queue state");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 1U,
        "studio.action.tasks.wait", "Wait for Idle", "Wait for queued work to finish");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 2U,
        "studio.action.tasks.retry", "Retry…", "Retry a failed operation node");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 3U,
        "studio.action.tasks.cancel", "Cancel…", "Cancel an operation node");
    return status;
}
