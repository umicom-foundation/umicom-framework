/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_graph.c
 *
 * PURPOSE:
 *   Verify workspace trust, project discovery, snapshots and project lookup
 *   against a temporary multi-project directory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_graph.h"

#include <assert.h>
#include <stdio.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char leaf[128];
    char project[UMI_PATH_CAPACITY];
    char cmake[UMI_PATH_CAPACITY];
    UmiWorkspaceGraph *graph = NULL;
    UmiWorkspaceGraphSnapshot snapshot;
    UmiWorkspaceProjectSnapshot project_snapshot;

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf,
                   sizeof(leaf),
                   "umicom-workspace-%llu",
                   (unsigned long long)umi_thread_current_id());
    assert(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "project-a", project, sizeof(project)) ==
           UMI_STATUS_OK);
    assert(umi_fs_make_directories(project) == UMI_STATUS_OK);
    assert(umi_path_join(project, "CMakeLists.txt", cmake, sizeof(cmake)) ==
           UMI_STATUS_OK);
    assert(umi_fs_write_text(cmake, "cmake_minimum_required(VERSION 3.24)") ==
           UMI_STATUS_OK);

    assert(umi_workspace_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_workspace_graph_open(graph, root, 1) == UMI_STATUS_OK);
    assert(umi_workspace_graph_discover(graph) == UMI_STATUS_OK);
    assert(umi_workspace_graph_snapshot(graph, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.open);
    assert(snapshot.trusted);
    assert(snapshot.project_count == 1U);
    assert(umi_workspace_graph_project_at(graph, 0U, &project_snapshot) ==
           UMI_STATUS_OK);
    assert(project_snapshot.kind == UMI_PROJECT_CMAKE);
    assert(umi_workspace_graph_find_project(graph,
                                            cmake,
                                            &project_snapshot) ==
           UMI_STATUS_OK);
    assert(umi_workspace_graph_close(graph) == UMI_STATUS_OK);
    umi_workspace_graph_destroy(graph);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
