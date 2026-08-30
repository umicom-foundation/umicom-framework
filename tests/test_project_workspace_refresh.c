/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace_refresh.c
 *
 * PURPOSE:
 *   Verify that workspace refresh is deterministic and non-destructive: it
 *   reports unchanged, new and missing projects without deleting membership.
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

int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceModel *model = NULL;
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectWorkspaceRootSnapshot root_snapshot = {0};
    UmiProjectWorkspaceMemberSnapshot member = {0};
    UmiProjectWorkspaceRefreshSnapshot refresh;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char nested[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];

    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK)
        return 1;
    if (umi_path_join(temporary, "umicom-b34-refresh", root, sizeof(root)) !=
        UMI_STATUS_OK) return 2;
    (void)umi_fs_remove_tree(root);
    if (umi_fs_make_directories(root) != UMI_STATUS_OK) return 3;
    if (umi_path_join(root, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK ||
        umi_fs_write_text(cmake_file, "project(root C)\n") != UMI_STATUS_OK)
        return 4;
    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 5;
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "root-project");
    strcpy(project.name, "Root Project");
    strcpy(project.root_uri, root);
    project.enabled = 1;
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace), &project) !=
        UMI_STATUS_OK) return 6;
    if (umi_project_workspace_model_create(workspace, &model) != UMI_STATUS_OK)
        return 7;
    root_snapshot.struct_size = (uint32_t)sizeof(root_snapshot);
    root_snapshot.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(root_snapshot.id, "main");
    strcpy(root_snapshot.path, root);
    root_snapshot.enabled = 1;
    if (umi_project_workspace_model_upsert_root(model, &root_snapshot) !=
        UMI_STATUS_OK) return 8;
    member.struct_size = (uint32_t)sizeof(member);
    member.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(member.id, "member.root");
    strcpy(member.root_id, "main");
    strcpy(member.project_id, "root-project");
    member.enabled = 1;
    if (umi_project_workspace_model_upsert_member(model, &member) !=
        UMI_STATUS_OK) return 9;

    if (umi_project_workspace_model_plan_refresh(model, NULL, &refresh) !=
        UMI_STATUS_OK) return 10;
    if (refresh.existing_project_count != 1U ||
        refresh.unchanged_project_count != 1U ||
        refresh.import_candidate_count != 0U ||
        refresh.missing_project_count != 0U || refresh.requires_confirmation)
        return 11;

    if (umi_path_join(root, "new-project", nested, sizeof(nested)) !=
            UMI_STATUS_OK ||
        umi_fs_make_directories(nested) != UMI_STATUS_OK ||
        umi_path_join(nested, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK ||
        umi_fs_write_text(cmake_file, "project(new_project C)\n") !=
            UMI_STATUS_OK) return 12;
    if (umi_project_workspace_model_plan_refresh(model, NULL, &refresh) !=
        UMI_STATUS_OK) return 13;
    if (refresh.unchanged_project_count != 1U ||
        refresh.import_candidate_count != 1U ||
        refresh.missing_project_count != 0U || !refresh.requires_confirmation ||
        umi_project_workspace_model_member_count(model) != 1U) return 14;

    strcpy(project.root_uri, "/path/which/does/not/exist");
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace), &project) !=
            UMI_STATUS_OK) return 15;
    if (umi_project_workspace_model_plan_refresh(model, NULL, &refresh) !=
        UMI_STATUS_OK) return 16;
    if (refresh.unchanged_project_count != 0U ||
        refresh.import_candidate_count != 2U ||
        refresh.missing_project_count != 1U ||
        umi_project_workspace_model_member_count(model) != 1U) return 17;

    umi_project_workspace_model_destroy(model);
    umi_project_workspace_destroy(workspace);
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 18;
    return 0;
}
