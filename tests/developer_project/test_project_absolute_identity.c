/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_project_absolute_identity.c
 * PURPOSE: Preserve explicit project paths when process cwd lookup is unavailable.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

static unsigned currentDirectoryCalls;
static UmiStatus MissingCurrentDirectory(char *output, size_t capacity)
{
    (void)output;
    (void)capacity;
    ++currentDirectoryCalls;
    return UMI_STATUS_NOT_FOUND;
}

/* Inject only cwd lookup into the two real implementations. Rename exported
 * symbols in this test translation unit so the normal Framework remains linked
 * unchanged; path parsing, directory checks and graph ownership stay real. */
#define umi_fs_current_directory MissingCurrentDirectory
#define UmiBuildProfileForWorkspace IdentityProfileForWorkspace
#define UmiBuildProfileFromProject IdentityProfileFromProject
#include "../../src/build/project_profile.c"
#undef UmiBuildProfileForWorkspace
#undef UmiBuildProfileFromProject
#define umi_workspace_graph_create IdentityGraph_create
#define umi_workspace_graph_destroy IdentityGraph_destroy
#define umi_workspace_graph_open IdentityGraph_open
#define umi_workspace_graph_close IdentityGraph_close
#define umi_workspace_graph_set_trusted IdentityGraph_set_trusted
#define umi_workspace_graph_add_project IdentityGraph_add_project
#define umi_workspace_graph_remove_project IdentityGraph_remove_project
#define umi_workspace_graph_discover IdentityGraph_discover
#define umi_workspace_graph_snapshot IdentityGraph_snapshot
#define umi_workspace_graph_project_at IdentityGraph_project_at
#define umi_workspace_graph_find_project IdentityGraph_find_project
#include "../../src/platform/workspace_graph.c"
#undef umi_workspace_graph_create
#undef umi_workspace_graph_destroy
#undef umi_workspace_graph_open
#undef umi_workspace_graph_close
#undef umi_workspace_graph_set_trusted
#undef umi_workspace_graph_add_project
#undef umi_workspace_graph_remove_project
#undef umi_workspace_graph_discover
#undef umi_workspace_graph_snapshot
#undef umi_workspace_graph_project_at
#undef umi_workspace_graph_find_project
#undef umi_fs_current_directory

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); failed = 1; goto cleanup; } } while (0)
int main(void)
{
    int failed = 0;
    UmiWorkspaceGraph *graph = NULL;
    char root[UMI_PATH_CAPACITY];
    UmiBuildProfile profile, before;
    UmiWorkspaceGraphSnapshot opened, after;
    /* The real cwd supplies an existing absolute fixture; the tested APIs see
     * a lookup failure and therefore cannot depend on that environmental value. */
    CHECK(umi_fs_current_directory(root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_path_is_absolute(root));
    umi_build_profile_init(&profile);
    CHECK(IdentityProfileForWorkspace(root, &profile) == UMI_STATUS_OK);
    CHECK(currentDirectoryCalls == 0U && umi_path_equal(profile.source_directory, root));
    before = profile;
    CHECK(IdentityProfileForWorkspace("relative-project", &profile) == UMI_STATUS_NOT_FOUND);
    CHECK(currentDirectoryCalls == 1U && memcmp(&before, &profile, sizeof profile) == 0);
    CHECK(IdentityGraph_create(&graph) == UMI_STATUS_OK);
    CHECK(IdentityGraph_open(graph, root, 0) == UMI_STATUS_OK);
    CHECK(currentDirectoryCalls == 1U);
    CHECK(IdentityGraph_snapshot(graph, &opened) == UMI_STATUS_OK);
    CHECK(opened.open && !opened.trusted && umi_path_equal(opened.root, root));
    CHECK(IdentityGraph_open(graph, "relative-project", 1) == UMI_STATUS_NOT_FOUND);
    CHECK(currentDirectoryCalls == 2U);
    CHECK(IdentityGraph_snapshot(graph, &after) == UMI_STATUS_OK);
    CHECK(after.open && !after.trusted && after.revision == opened.revision &&
        umi_path_equal(after.root, opened.root));
cleanup:
    IdentityGraph_destroy(graph);
    return failed;
}
