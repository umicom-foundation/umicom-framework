/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/action_recovery/test_discovery_interference.c
 * PURPOSE: Inject changes at the end of real discovery before publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_graph.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static UmiWorkspaceGraph *liveGraph;
static const char *testMode;
static char replacementRoot[UMI_PATH_CAPACITY];
static UmiWorkspaceGraphSnapshot intervening;
static int failAllocation;

void *__real_calloc(size_t count, size_t size);
void *__wrap_calloc(size_t count, size_t size)
{
    if (failAllocation) { failAllocation = 0; return NULL; }
    return __real_calloc(count, size);
}
UmiStatus __real_umi_directory_walk(const char *root,
    const UmiDirectoryWalkOptions *options, UmiDirectoryVisitor visitor, void *userData);
UmiStatus __wrap_umi_directory_walk(const char *root,
    const UmiDirectoryWalkOptions *options, UmiDirectoryVisitor visitor, void *userData)
{
    UmiStatus status = __real_umi_directory_walk(root, options, visitor, userData);
    if (status != UMI_STATUS_OK) return status;
    /* The actual walker and visitor run first. Only the completion boundary is
     * injected: this is deterministic interference, not a thread stress test. */
    if (strcmp(testMode, "read-error") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(testMode, "trust-change") == 0)
        status = umi_workspace_graph_set_trusted(liveGraph, 0);
    else if (strcmp(testMode, "workspace-change") == 0)
        status = umi_workspace_graph_open(liveGraph, replacementRoot, 0);
    else if (strcmp(testMode, "close") == 0)
        status = umi_workspace_graph_close(liveGraph);
    if (status != UMI_STATUS_OK) return status;
    return umi_workspace_graph_snapshot(liveGraph, &intervening);
}

int main(int argc, char **argv)
{
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], child[UMI_PATH_CAPACITY];
    char marker[UMI_PATH_CAPACITY], leaf[96];
    UmiWorkspaceGraphSnapshot before, after;
    UmiStatus status;
    REQUIRE(argc == 2);
    testMode = argv[1];
    REQUIRE(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-discovery-interference-%ld", (long)getpid());
    REQUIRE(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_path_join(root, "replacement", replacementRoot, sizeof(replacementRoot)) == UMI_STATUS_OK);
    REQUIRE(umi_fs_make_directories(replacementRoot) == UMI_STATUS_OK);
    REQUIRE(umi_path_join(root, "notes", child, sizeof(child)) == UMI_STATUS_OK);
    REQUIRE(umi_fs_make_directories(child) == UMI_STATUS_OK);
    REQUIRE(umi_path_join(child, "CMakeLists.txt", marker, sizeof(marker)) == UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(marker, "cmake_minimum_required(VERSION 3.24)\n") == UMI_STATUS_OK);
    REQUIRE(umi_workspace_graph_create(&liveGraph) == UMI_STATUS_OK);
    REQUIRE(umi_workspace_graph_open(liveGraph, root, 1) == UMI_STATUS_OK);
    REQUIRE(umi_workspace_graph_snapshot(liveGraph, &before) == UMI_STATUS_OK);
    if (strcmp(testMode, "allocation") == 0) failAllocation = 1;
    status = umi_workspace_graph_discover(liveGraph);
    REQUIRE(umi_workspace_graph_snapshot(liveGraph, &after) == UMI_STATUS_OK);
    if (strcmp(testMode, "read-error") == 0 || strcmp(testMode, "allocation") == 0) {
        REQUIRE(status == (strcmp(testMode, "allocation") == 0 ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_IO_ERROR));
        REQUIRE(memcmp(&before, &after, sizeof(before)) == 0);
    } else {
        REQUIRE(status == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&intervening, &after, sizeof(after)) == 0);
        REQUIRE(after.project_count == 0U);
        if (strcmp(testMode, "workspace-change") == 0) REQUIRE(strcmp(after.root, replacementRoot) == 0);
        if (strcmp(testMode, "close") == 0) REQUIRE(!after.open);
        if (strcmp(testMode, "trust-change") == 0) REQUIRE(!after.trusted);
    }
    umi_workspace_graph_destroy(liveGraph);
    REQUIRE(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    puts(testMode);
    return 0;
}
