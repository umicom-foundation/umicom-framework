/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/task_graph.h
 *
 * PURPOSE:
 *   Convert declarative workspace tasks into a dependency-aware, deterministic
 *   execution graph with explicit readiness, failure propagation and retry state.
 *
 * ARCHITECTURE:
 *   The graph copies task records from a registry. It does not spawn processes.
 *   A task runner or orchestration service asks for ready tasks, records lifecycle
 *   transitions and translates cooperative cancellation to a process provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_TASK_GRAPH_H
#define UMICOM_BUILD_TASK_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/build/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_TASK_GRAPH_API_VERSION 1U

typedef enum UmiBuildTaskState {
    UMI_BUILD_TASK_PENDING = 0,
    UMI_BUILD_TASK_READY = 1,
    UMI_BUILD_TASK_RUNNING = 2,
    UMI_BUILD_TASK_SUCCEEDED = 3,
    UMI_BUILD_TASK_FAILED = 4,
    UMI_BUILD_TASK_CANCELLED = 5,
    UMI_BUILD_TASK_SKIPPED = 6,
    UMI_BUILD_TASK_BLOCKED = 7
} UmiBuildTaskState;

typedef struct UmiBuildTaskGraphNodeSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildTaskSnapshot task;
    UmiBuildTaskState state;
    size_t satisfied_dependency_count;
    size_t failed_dependency_count;
    uint32_t progress_basis_points;
    uint32_t attempt_count;
    UmiStatus last_status;
    int last_exit_code;
    uint64_t revision;
} UmiBuildTaskGraphNodeSnapshot;

typedef struct UmiBuildTaskGraphSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t task_count;
    size_t dependency_count;
    size_t pending_count;
    size_t ready_count;
    size_t running_count;
    size_t succeeded_count;
    size_t failed_count;
    size_t cancelled_count;
    size_t skipped_count;
    size_t blocked_count;
    uint32_t progress_basis_points;
    uint64_t source_revision;
    uint64_t revision;
    int cyclic;
} UmiBuildTaskGraphSnapshot;

typedef struct UmiBuildTaskGraph UmiBuildTaskGraph;

UmiStatus umi_build_task_graph_create(UmiBuildTaskGraph **out_graph);
void umi_build_task_graph_destroy(UmiBuildTaskGraph *graph);
UmiStatus umi_build_task_graph_rebuild(
    UmiBuildTaskGraph *graph,
    const UmiBuildTaskRegistry *registry);
UmiStatus umi_build_task_graph_reset(UmiBuildTaskGraph *graph);
UmiStatus umi_build_task_graph_refresh(UmiBuildTaskGraph *graph);
UmiStatus umi_build_task_graph_find(
    const UmiBuildTaskGraph *graph,
    const char *task_id,
    UmiBuildTaskGraphNodeSnapshot *out_node);
UmiStatus umi_build_task_graph_at(
    const UmiBuildTaskGraph *graph,
    size_t position,
    UmiBuildTaskGraphNodeSnapshot *out_node);
UmiStatus umi_build_task_graph_next_ready(
    const UmiBuildTaskGraph *graph,
    UmiBuildTaskGraphNodeSnapshot *out_node);
UmiStatus umi_build_task_graph_start(
    UmiBuildTaskGraph *graph,
    const char *task_id);
UmiStatus umi_build_task_graph_set_progress(
    UmiBuildTaskGraph *graph,
    const char *task_id,
    uint32_t progress_basis_points);
UmiStatus umi_build_task_graph_complete(
    UmiBuildTaskGraph *graph,
    const char *task_id,
    int exit_code);
UmiStatus umi_build_task_graph_fail(
    UmiBuildTaskGraph *graph,
    const char *task_id,
    UmiStatus status,
    int exit_code);
UmiStatus umi_build_task_graph_cancel(
    UmiBuildTaskGraph *graph,
    const char *task_id);
UmiStatus umi_build_task_graph_retry(
    UmiBuildTaskGraph *graph,
    const char *task_id);
UmiStatus umi_build_task_graph_skip(
    UmiBuildTaskGraph *graph,
    const char *task_id);
UmiStatus umi_build_task_graph_cancel_waiting(UmiBuildTaskGraph *graph);
UmiStatus umi_build_task_graph_snapshot(
    const UmiBuildTaskGraph *graph,
    UmiBuildTaskGraphSnapshot *out_snapshot);
size_t umi_build_task_graph_count(const UmiBuildTaskGraph *graph);
uint64_t umi_build_task_graph_revision(const UmiBuildTaskGraph *graph);
const char *umi_build_task_state_text(UmiBuildTaskState state);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_TASK_GRAPH_H */
