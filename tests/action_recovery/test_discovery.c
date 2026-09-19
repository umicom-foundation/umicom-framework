/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/action_recovery/test_discovery.c
 * PURPOSE: Check atomic project discovery using real temporary folders.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_graph.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define TEST_PROCESS_ID _getpid()
#else
#include <unistd.h>
#define TEST_PROCESS_ID getpid()
#endif
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* Create two discoverable projects; this operation never touches a user project. */
static UmiStatus MakeProject(const char *root, const char *name)
{
    char path[UMI_PATH_CAPACITY];
    char marker[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(root, name, path, sizeof(path));
    if (status == UMI_STATUS_OK) status = umi_fs_make_directories(path);
    if (status == UMI_STATUS_OK) status = umi_path_join(path, "CMakeLists.txt", marker, sizeof(marker));
    if (status == UMI_STATUS_OK) status = umi_fs_write_text(marker, "cmake_minimum_required(VERSION 3.24)\n");
    return status;
}

int main(int argc, char **argv)
{
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], name[96];
    UmiWorkspaceGraph *graph = NULL;
    UmiWorkspaceGraphSnapshot before, after;
    UmiWorkspaceProjectSnapshot project;
    uint64_t lastId = 0U;
    const char *mode = argc > 1 ? argv[1] : "success";
    REQUIRE(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(name, sizeof(name), "umicom-discovery-recovery-%ld", (long)TEST_PROCESS_ID);
    REQUIRE(umi_path_join(temporary, name, root, sizeof(root)) == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root) == UMI_STATUS_OK);
    REQUIRE(umi_workspace_graph_create(&graph) == UMI_STATUS_OK);
    REQUIRE(umi_workspace_graph_open(graph, root, 1) == UMI_STATUS_OK);

    if (strcmp(mode, "capacity") == 0) {
        char path[UMI_PATH_CAPACITY];
        for (size_t index = 0U; index < UMI_WORKSPACE_PROJECT_MAX - 1U; ++index) {
            (void)snprintf(name, sizeof(name), "manual-%zu", index);
            REQUIRE(umi_path_join(root, name, path, sizeof(path)) == UMI_STATUS_OK);
            REQUIRE(umi_workspace_graph_add_project(graph, name, name, path,
                UMI_PROJECT_GENERIC, &lastId) == UMI_STATUS_OK);
        }
        REQUIRE(MakeProject(root, "notes-a") == UMI_STATUS_OK);
        REQUIRE(MakeProject(root, "notes-b") == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_snapshot(graph, &before) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_discover(graph) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_workspace_graph_snapshot(graph, &after) == UMI_STATUS_OK);
        REQUIRE(after.project_count == before.project_count);
        REQUIRE(after.revision == before.revision);
        REQUIRE(umi_workspace_graph_project_at(graph, before.project_count - 1U, &project) == UMI_STATUS_OK);
        REQUIRE(project.project_id == lastId);
        /* A rejected candidate must not consume IDs from the published graph. */
        REQUIRE(umi_workspace_graph_remove_project(graph, lastId) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_discover(graph) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_project_at(graph, before.project_count - 1U, &project) == UMI_STATUS_OK);
        REQUIRE(project.project_id == lastId + 1U);
    } else if (strcmp(mode, "missing-root") == 0) {
        REQUIRE(umi_workspace_graph_snapshot(graph, &before) == UMI_STATUS_OK);
        REQUIRE(umi_fs_remove_tree(root) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_discover(graph) != UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_snapshot(graph, &after) == UMI_STATUS_OK);
        REQUIRE(after.project_count == before.project_count && after.revision == before.revision);
    } else if (strcmp(mode, "closed") == 0) {
        REQUIRE(umi_workspace_graph_close(graph) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_discover(graph) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "success") == 0 || strcmp(mode, "unchanged") == 0) {
        REQUIRE(MakeProject(root, "notes-a") == UMI_STATUS_OK);
        REQUIRE(MakeProject(root, "notes-b") == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_snapshot(graph, &before) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_discover(graph) == UMI_STATUS_OK);
        REQUIRE(umi_workspace_graph_snapshot(graph, &after) == UMI_STATUS_OK);
        REQUIRE(after.project_count == 2U);
        REQUIRE(after.revision == before.revision + 1U);
        REQUIRE(umi_workspace_graph_project_at(graph, 0U, &project) == UMI_STATUS_OK);
        REQUIRE(strcmp(project.display_name, "notes-a") == 0);
        REQUIRE(project.kind == UMI_PROJECT_CMAKE);
        if (strcmp(mode, "unchanged") == 0) {
            before = after;
            REQUIRE(umi_workspace_graph_discover(graph) == UMI_STATUS_OK);
            REQUIRE(umi_workspace_graph_snapshot(graph, &after) == UMI_STATUS_OK);
            REQUIRE(after.revision == before.revision && after.project_count == before.project_count);
        }
    } else { fprintf(stderr, "Unknown test case\n"); return 2; }
    umi_workspace_graph_destroy(graph);
    REQUIRE(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    puts(mode);
    return 0;
}
