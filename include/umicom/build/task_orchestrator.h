/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/task_orchestrator.h
 *
 * PURPOSE:
 *   Plan and supervise one dependency-closed task operation with bounded
 *   parallelism, retry metadata, failure propagation and cooperative stop.
 *
 * ARCHITECTURE:
 *   The orchestrator creates an isolated graph from a source task registry.
 *   Process execution stays outside this module so the same plan can drive a
 *   local shell, remote host, container, test adapter or graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_TASK_ORCHESTRATOR_H
#define UMICOM_BUILD_TASK_ORCHESTRATOR_H

#include "umicom/build/task_graph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_TASK_ORCHESTRATOR_API_VERSION 1U

typedef enum UmiBuildTaskOperationState {
    UMI_BUILD_TASK_OPERATION_IDLE = 0,
    UMI_BUILD_TASK_OPERATION_PLANNED = 1,
    UMI_BUILD_TASK_OPERATION_RUNNING = 2,
    UMI_BUILD_TASK_OPERATION_SUCCEEDED = 3,
    UMI_BUILD_TASK_OPERATION_FAILED = 4,
    UMI_BUILD_TASK_OPERATION_CANCELLED = 5
} UmiBuildTaskOperationState;

typedef struct UmiBuildTaskOperationSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char operation_id[UMI_BUILD_ID_CAPACITY];
    char requested_task_id[UMI_BUILD_ID_CAPACITY];
    UmiBuildTaskOperationState state;
    UmiBuildTaskGraphSnapshot graph;
    size_t maximum_parallel;
    size_t active_count;
    size_t planned_count;
    size_t completed_count;
    size_t retry_count;
    uint64_t generation;
    uint64_t revision;
    int stop_requested;
} UmiBuildTaskOperationSnapshot;

typedef struct UmiBuildTaskOrchestrator UmiBuildTaskOrchestrator;

UmiStatus umi_build_task_orchestrator_create(
    const UmiBuildTaskRegistry *source_registry,
    size_t maximum_parallel,
    UmiBuildTaskOrchestrator **out_orchestrator);
void umi_build_task_orchestrator_destroy(
    UmiBuildTaskOrchestrator *orchestrator);
UmiStatus umi_build_task_orchestrator_plan(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *operation_id,
    const char *requested_task_id);
UmiStatus umi_build_task_orchestrator_begin(
    UmiBuildTaskOrchestrator *orchestrator);
UmiStatus umi_build_task_orchestrator_next_ready(
    UmiBuildTaskOrchestrator *orchestrator,
    UmiBuildTaskSnapshot *out_task);
UmiStatus umi_build_task_orchestrator_start(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id);
UmiStatus umi_build_task_orchestrator_set_progress(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    uint32_t progress_basis_points);
UmiStatus umi_build_task_orchestrator_complete(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    int exit_code);
UmiStatus umi_build_task_orchestrator_fail(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id,
    UmiStatus status,
    int exit_code);
UmiStatus umi_build_task_orchestrator_retry(
    UmiBuildTaskOrchestrator *orchestrator,
    const char *task_id);
UmiStatus umi_build_task_orchestrator_request_stop(
    UmiBuildTaskOrchestrator *orchestrator);
UmiStatus umi_build_task_orchestrator_finish(
    UmiBuildTaskOrchestrator *orchestrator);
UmiStatus umi_build_task_orchestrator_snapshot(
    const UmiBuildTaskOrchestrator *orchestrator,
    UmiBuildTaskOperationSnapshot *out_snapshot);
UmiBuildTaskGraph *umi_build_task_orchestrator_graph(
    UmiBuildTaskOrchestrator *orchestrator);
const char *umi_build_task_operation_state_text(
    UmiBuildTaskOperationState state);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_TASK_ORCHESTRATOR_H */
