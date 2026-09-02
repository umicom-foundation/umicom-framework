/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace_discovery.c
 *
 * PURPOSE:
 *   Protect Batch 34 multi-root CMake discovery, root-project detection,
 *   nested-project policy, build metadata detection and exclusion rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/project/project.h"

/*
 * Exercise join path and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int join_path(const char *left, const char *right,
                     char *out_path, size_t capacity)
{
    return umi_path_join(left, right, out_path, capacity) == UMI_STATUS_OK
        ? 0 : 1;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceModel *model = NULL;
    UmiProjectWorkspaceRootSnapshot workspace_root = {0};
    UmiProjectWorkspaceExclusionSnapshot exclusion = {0};
    UmiProjectWorkspaceDiscoveryOptions options;
    UmiProjectWorkspaceDiscoverySnapshot discovery;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char nested[UMI_PATH_CAPACITY];
    char ignored[UMI_PATH_CAPACITY];
    char build[UMI_PATH_CAPACITY];
    char git[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK)
        return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(temporary, "umicom-b34-discovery", root, sizeof(root)))
        return 2;
    (void)umi_fs_remove_tree(root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_make_directories(root) != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(root, "nested", nested, sizeof(nested)) ||
        join_path(root, "ignored", ignored, sizeof(ignored)) ||
        join_path(root, "build", build, sizeof(build)) ||
        join_path(root, ".git", git, sizeof(git))) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_make_directories(nested) != UMI_STATUS_OK ||
        umi_fs_make_directories(ignored) != UMI_STATUS_OK ||
        umi_fs_make_directories(build) != UMI_STATUS_OK ||
        umi_fs_make_directories(git) != UMI_STATUS_OK) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(root, "CMakeLists.txt", path, sizeof(path)) ||
        umi_fs_write_text(path, "project(root C)\n") != UMI_STATUS_OK)
        return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(root, "CMakePresets.json", path, sizeof(path)) ||
        umi_fs_write_text(path, "{\"version\": 3}\n") != UMI_STATUS_OK)
        return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(nested, "CMakeLists.txt", path, sizeof(path)) ||
        umi_fs_write_text(path, "project(nested C)\n") != UMI_STATUS_OK)
        return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(ignored, "CMakeLists.txt", path, sizeof(path)) ||
        umi_fs_write_text(path, "project(ignored C)\n") != UMI_STATUS_OK)
        return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (join_path(build, "compile_commands.json", path, sizeof(path)) ||
        umi_fs_write_text(path, "[]\n") != UMI_STATUS_OK) return 10;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 11;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_model_create(workspace, &model) != UMI_STATUS_OK)
        return 12;
    workspace_root.struct_size = (uint32_t)sizeof(workspace_root);
    workspace_root.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(workspace_root.id, "fixture");
    strcpy(workspace_root.label, "Discovery fixture");
    strcpy(workspace_root.path, root);
    workspace_root.enabled = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_upsert_root(model, &workspace_root) !=
        UMI_STATUS_OK) return 13;
    exclusion.struct_size = (uint32_t)sizeof(exclusion);
    exclusion.api_version = UMI_PROJECT_WORKSPACE_EXCLUSION_API_VERSION;
    strcpy(exclusion.id, "ignore-fixture");
    strcpy(exclusion.pattern, "ignored");
    exclusion.kind = UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT;
    exclusion.enabled = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_upsert_exclusion(model, &exclusion) !=
        UMI_STATUS_OK) return 14;

    options = umi_project_workspace_discovery_options_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_discover(model, &options, &discovery) !=
        UMI_STATUS_OK) return 15;
    /* Apply this branch only when its contract condition is satisfied. */
    if (discovery.project_count != 2U ||
        discovery.excluded_directory_count == 0U ||
        !discovery.projects[0].has_cmake ||
        !discovery.projects[0].has_presets ||
        !discovery.projects[0].has_build_directory ||
        !discovery.projects[0].has_compile_commands ||
        !discovery.projects[0].has_git ||
        !umi_path_equal(discovery.projects[0].project_directory, root))
        return 16;

    options.include_nested_projects = 0;
    options.detect_build_directories = 0;
    options.detect_git_repositories = 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_discover(model, &options, &discovery) !=
        UMI_STATUS_OK) return 17;
    /* Apply this branch only when its contract condition is satisfied. */
    if (discovery.project_count != 1U ||
        discovery.projects[0].has_build_directory ||
        discovery.projects[0].has_git) return 18;

    umi_project_workspace_model_destroy(model);
    umi_project_workspace_destroy(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 19;
    return 0;
}
