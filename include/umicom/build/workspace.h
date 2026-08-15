/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/workspace.h
 *
 * PURPOSE:
 *   Coordinate the provider-neutral build graph, retained operation history,
 *   produced artifacts and shared background task queue behind one stable C
 *   contract suitable for desktop, web, headless and automation frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The workspace borrows the build objects supplied in its bindings. It never
 * executes a compiler by itself and never owns or destroys those services.
 * Applications remain responsible for execution while every frontend observes
 * the same selection, filtering, history and command-enablement state.
 */
#ifndef UMICOM_BUILD_WORKSPACE_H
#define UMICOM_BUILD_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/build/artifact.h"
#include "umicom/build/graph.h"
#include "umicom/build/history.h"
#include "umicom/build/profile.h"
#include "umicom/platform/task_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_WORKSPACE_API_VERSION 1U
#define UMI_BUILD_WORKSPACE_FILTER_CAPACITY 160U

typedef enum UmiBuildWorkspaceNodeFilter {
    UMI_BUILD_WORKSPACE_NODES_ALL = 0,
    UMI_BUILD_WORKSPACE_NODES_PENDING = 1,
    UMI_BUILD_WORKSPACE_NODES_READY = 2,
    UMI_BUILD_WORKSPACE_NODES_RUNNING = 3,
    UMI_BUILD_WORKSPACE_NODES_SUCCEEDED = 4,
    UMI_BUILD_WORKSPACE_NODES_FAILED = 5,
    UMI_BUILD_WORKSPACE_NODES_BLOCKED = 6
} UmiBuildWorkspaceNodeFilter;

typedef struct UmiBuildWorkspaceBindings {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildGraph *graph;
    UmiBuildHistory *history;
    UmiBuildArtifactIndex *artifacts;
    UmiTaskQueue *task_queue;
    const UmiBuildProfile *profile;
} UmiBuildWorkspaceBindings;

typedef struct UmiBuildWorkspaceSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildGraphSnapshot graph;
    UmiTaskQueueStats tasks;
    char profile_id[UMI_BUILD_ID_CAPACITY];
    char source_directory[UMI_BUILD_PATH_CAPACITY];
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    char configuration[UMI_BUILD_NAME_CAPACITY];
    char generator[UMI_BUILD_NAME_CAPACITY];
    char build_target[UMI_BUILD_NAME_CAPACITY];
    char filter_text[UMI_BUILD_WORKSPACE_FILTER_CAPACITY];
    UmiBuildWorkspaceNodeFilter node_filter;
    char selected_node_id[UMI_BUILD_ID_CAPACITY];
    uint64_t selected_operation_id;
    char selected_artifact_id[UMI_BUILD_ID_CAPACITY];
    size_t visible_node_count;
    size_t history_count;
    size_t artifact_count;
    UmiBuildPhase latest_phase;
    UmiBuildState latest_state;
    UmiStatus latest_status;
    int latest_exit_code;
    uint64_t latest_duration_ms;
    size_t latest_diagnostic_count;
    uint64_t revision;
    int has_latest_result;
    int has_selected_node;
    int has_selected_operation;
    int has_selected_artifact;
    int can_execute_next;
    int can_execute_all;
    int can_retry_selected;
    int can_cancel;
    int can_clear_history;
} UmiBuildWorkspaceSnapshot;

typedef struct UmiBuildWorkspace UmiBuildWorkspace;

void umi_build_workspace_bindings_init(UmiBuildWorkspaceBindings *bindings);
UmiStatus umi_build_workspace_create(
    const UmiBuildWorkspaceBindings *bindings,
    UmiBuildWorkspace **out_workspace);
void umi_build_workspace_destroy(UmiBuildWorkspace *workspace);

UmiStatus umi_build_workspace_set_task_queue(
    UmiBuildWorkspace *workspace,
    UmiTaskQueue *task_queue);
UmiStatus umi_build_workspace_set_filter(
    UmiBuildWorkspace *workspace,
    const char *text,
    UmiBuildWorkspaceNodeFilter node_filter);
UmiStatus umi_build_workspace_select_node(
    UmiBuildWorkspace *workspace,
    const char *node_id);
UmiStatus umi_build_workspace_select_operation(
    UmiBuildWorkspace *workspace,
    uint64_t operation_id);
UmiStatus umi_build_workspace_select_latest_operation(
    UmiBuildWorkspace *workspace);
UmiStatus umi_build_workspace_select_artifact(
    UmiBuildWorkspace *workspace,
    const char *artifact_id);
UmiStatus umi_build_workspace_refresh(UmiBuildWorkspace *workspace);
void umi_build_workspace_clear_history(UmiBuildWorkspace *workspace);

UmiStatus umi_build_workspace_snapshot(
    UmiBuildWorkspace *workspace,
    UmiBuildWorkspaceSnapshot *out_snapshot);
UmiStatus umi_build_workspace_visible_node_at(
    UmiBuildWorkspace *workspace,
    size_t index,
    UmiBuildGraphNodeSnapshot *out_node);
UmiStatus umi_build_workspace_history_at(
    UmiBuildWorkspace *workspace,
    size_t newest_first_index,
    UmiBuildResult *out_result);
UmiStatus umi_build_workspace_artifact_at(
    UmiBuildWorkspace *workspace,
    size_t index,
    UmiBuildArtifactSnapshot *out_artifact);
UmiStatus umi_build_workspace_selected_result(
    UmiBuildWorkspace *workspace,
    UmiBuildResult *out_result);
UmiStatus umi_build_workspace_selected_artifact(
    UmiBuildWorkspace *workspace,
    UmiBuildArtifactSnapshot *out_artifact);

const char *umi_build_workspace_node_filter_text(
    UmiBuildWorkspaceNodeFilter node_filter);

#ifdef __cplusplus
}
#endif

#endif
