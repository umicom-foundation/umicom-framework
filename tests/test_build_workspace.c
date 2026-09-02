/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_workspace.c
 *
 * PURPOSE:
 *   Verify professional build graph filtering, stable selections, retained
 *   operation history, artifacts and capability-aware command state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/build/workspace.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildGraph *graph = NULL;
    UmiBuildHistory *history = NULL;
    UmiBuildArtifactIndex *artifacts = NULL;
    UmiTaskQueue *tasks = NULL;
    UmiBuildWorkspace *workspace = NULL;
    UmiBuildWorkspaceBindings bindings;
    UmiBuildWorkspaceSnapshot snapshot;
    UmiBuildGraphNodeSnapshot configure;
    UmiBuildGraphNodeSnapshot compile;
    UmiBuildGraphNodeSnapshot visible;
    UmiBuildProfile profile;
    UmiBuildResult *result = NULL;
    UmiBuildResult *selected_result = NULL;
    UmiBuildArtifactSnapshot artifact = {0};
    UmiBuildArtifactSnapshot selected_artifact;
    UmiTaskQueueConfig task_config = {1U, 4U};

    assert(umi_build_result_create(&result) == UMI_STATUS_OK);
    assert(umi_build_result_create(&selected_result) == UMI_STATUS_OK);
    assert(umi_build_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_build_history_create(8U, &history) == UMI_STATUS_OK);
    assert(umi_build_artifact_index_create(&artifacts) == UMI_STATUS_OK);
    assert(umi_task_queue_create(&task_config, &tasks) == UMI_STATUS_OK);
    umi_build_profile_init(&profile);
    assert(umi_build_profile_set(&profile, "development", ".",
                                 "build/debug") == UMI_STATUS_OK);
    (void)strcpy(profile.generator, "Ninja");
    (void)strcpy(profile.configuration, "Debug");

    umi_build_graph_node_init(&configure, "configure", "Configure",
                              UMI_BUILD_PHASE_CONFIGURE);
    umi_build_graph_node_init(&compile, "build", "Compile",
                              UMI_BUILD_PHASE_BUILD);
    assert(umi_build_graph_add_node(graph, &configure) == UMI_STATUS_OK);
    assert(umi_build_graph_add_node(graph, &compile) == UMI_STATUS_OK);
    assert(umi_build_graph_add_dependency(graph, "build", "configure") ==
           UMI_STATUS_OK);
    assert(umi_build_graph_refresh(graph) == UMI_STATUS_OK);

    umi_build_result_init(result, 42U, UMI_BUILD_PHASE_BUILD, "development");
    (void)strcpy(result->command, "cmake --build build/debug");
    (void)strcpy(result->output, "compiler output");
    umi_build_result_finish(result, UMI_STATUS_IO_ERROR, 1, 125U);
    assert(umi_build_history_append(history, result) == UMI_STATUS_OK);

    artifact.operation_id = 42U;
    (void)strcpy(artifact.node_id, "build");
    (void)strcpy(artifact.artifact_id, "studio.executable");
    (void)strcpy(artifact.path, "build/debug/bin/umicom-studio-ide");
    (void)strcpy(artifact.kind, "executable");
    artifact.size_bytes = 4096U;
    assert(umi_build_artifact_index_upsert(artifacts, &artifact) ==
           UMI_STATUS_OK);

    umi_build_workspace_bindings_init(&bindings);
    bindings.graph = graph;
    bindings.history = history;
    bindings.artifacts = artifacts;
    bindings.task_queue = tasks;
    bindings.profile = &profile;
    assert(umi_build_workspace_create(&bindings, &workspace) == UMI_STATUS_OK);
    assert(umi_build_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.graph.node_count == 2U);
    assert(snapshot.visible_node_count == 2U);
    assert(snapshot.history_count == 1U);
    assert(snapshot.artifact_count == 1U);
    assert(snapshot.tasks.worker_count == 1U);
    assert(snapshot.tasks.capacity == 4U);
    assert(snapshot.selected_operation_id == 42U);
    assert(strcmp(snapshot.selected_artifact_id, "studio.executable") == 0);
    assert(snapshot.can_execute_next);

    assert(umi_build_workspace_set_filter(
               workspace, "compile", UMI_BUILD_WORKSPACE_NODES_ALL) ==
           UMI_STATUS_OK);
    assert(umi_build_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.visible_node_count == 1U);
    assert(strcmp(snapshot.selected_node_id, "build") == 0);
    assert(umi_build_workspace_visible_node_at(workspace, 0U, &visible) ==
           UMI_STATUS_OK);
    assert(strcmp(visible.node_id, "build") == 0);

    assert(umi_build_graph_start(graph, "configure") == UMI_STATUS_OK);
    assert(umi_build_graph_complete(graph, "configure", 0) == UMI_STATUS_OK);
    assert(umi_build_graph_start(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_graph_fail(graph, "build", UMI_STATUS_IO_ERROR, 1) ==
           UMI_STATUS_OK);
    assert(umi_build_workspace_refresh(workspace) == UMI_STATUS_OK);
    assert(umi_build_workspace_select_node(workspace, "build") ==
           UMI_STATUS_OK);
    assert(umi_build_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.can_retry_selected);
    assert(umi_build_workspace_selected_result(workspace, selected_result) ==
           UMI_STATUS_OK);
    assert(selected_result->operation_id == 42U);
    assert(umi_build_workspace_selected_artifact(
               workspace, &selected_artifact) == UMI_STATUS_OK);
    assert(strcmp(selected_artifact.artifact_id, "studio.executable") == 0);

    umi_build_workspace_clear_history(workspace);
    assert(umi_build_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.history_count == 0U);
    assert(!snapshot.has_selected_operation);
    assert(!snapshot.can_clear_history);

    umi_build_workspace_destroy(workspace);
    umi_build_artifact_index_destroy(artifacts);
    umi_build_history_destroy(history);
    umi_build_graph_destroy(graph);
    umi_task_queue_destroy(tasks);
    umi_build_result_destroy(selected_result);
    umi_build_result_destroy(result);
    return 0;
}
