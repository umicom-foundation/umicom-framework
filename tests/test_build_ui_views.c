/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_ui_views.c
 *
 * PURPOSE:
 *   Verify every professional build-and-task pane projects one shared
 *   Framework workspace and exposes capability-aware command actions.
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

#include "umicom/build_ui/build_ui.h"
#include "umicom/ui/command_view.h"

typedef UmiStatus (*ViewFactory)(
    const char *, UmiBuildWorkspace *, UmiUiViewModel **);

static void verify_view(ViewFactory factory, const char *view_id,
                        const char *expected_kind,
                        UmiBuildWorkspace *workspace)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(factory(view_id, workspace, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiBuildGraph *graph = NULL;
    UmiBuildHistory *history = NULL;
    UmiBuildArtifactIndex *artifacts = NULL;
    UmiBuildWorkspace *workspace = NULL;
    UmiBuildWorkspaceBindings bindings;
    UmiBuildProfile profile;
    UmiBuildGraphNodeSnapshot node;
    UmiBuildResult *result = NULL;
    UmiBuildArtifactSnapshot artifact = {0};
    UmiUiViewModel *dashboard = NULL;
    UmiUiCommandViewAction action;

    assert(umi_build_result_create(&result) == UMI_STATUS_OK);
    assert(umi_build_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_build_history_create(8U, &history) == UMI_STATUS_OK);
    assert(umi_build_artifact_index_create(&artifacts) == UMI_STATUS_OK);
    umi_build_profile_init(&profile);
    assert(umi_build_profile_set(&profile, "development", ".",
                                 "build/debug") == UMI_STATUS_OK);
    (void)strcpy(profile.generator, "Ninja");
    (void)strcpy(profile.configuration, "Debug");
    umi_build_graph_node_init(&node, "configure", "Configure",
                              UMI_BUILD_PHASE_CONFIGURE);
    assert(umi_build_graph_add_node(graph, &node) == UMI_STATUS_OK);
    assert(umi_build_graph_refresh(graph) == UMI_STATUS_OK);
    umi_build_result_init(result, 7U, UMI_BUILD_PHASE_CONFIGURE,
                          profile.profile_id);
    (void)strcpy(result->command, "cmake --preset debug");
    (void)strcpy(result->output, "configured");
    umi_build_result_finish(result, UMI_STATUS_OK, 0, 25U);
    assert(umi_build_history_append(history, result) == UMI_STATUS_OK);
    artifact.operation_id = 7U;
    (void)strcpy(artifact.node_id, "configure");
    (void)strcpy(artifact.artifact_id, "cmake.cache");
    (void)strcpy(artifact.path, "build/debug/CMakeCache.txt");
    (void)strcpy(artifact.kind, "configuration");
    assert(umi_build_artifact_index_upsert(artifacts, &artifact) ==
           UMI_STATUS_OK);

    umi_build_workspace_bindings_init(&bindings);
    bindings.graph = graph;
    bindings.history = history;
    bindings.artifacts = artifacts;
    bindings.profile = &profile;
    assert(umi_build_workspace_create(&bindings, &workspace) == UMI_STATUS_OK);

    verify_view(umi_build_ui_dashboard_view_create, "dashboard",
                "build-dashboard", workspace);
    verify_view(umi_build_ui_graph_view_create, "graph", "build-graph",
                workspace);
    verify_view(umi_build_ui_history_view_create, "history", "build-history",
                workspace);
    verify_view(umi_build_ui_output_view_create, "output", "build-output",
                workspace);
    verify_view(umi_build_ui_artifacts_view_create, "artifacts",
                "build-artifacts", workspace);
    verify_view(umi_build_ui_tasks_view_create, "tasks", "build-tasks",
                workspace);

    assert(umi_build_ui_dashboard_view_create(
               "dashboard.actions", workspace, &dashboard) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(dashboard, 6U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.run-all") == 0);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(dashboard, 7U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.cancel") == 0);
    assert(!action.enabled);
    umi_ui_view_model_destroy(dashboard);

    umi_build_workspace_destroy(workspace);
    umi_build_artifact_index_destroy(artifacts);
    umi_build_history_destroy(history);
    umi_build_graph_destroy(graph);
    umi_build_result_destroy(result);
    return 0;
}
