/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace.c
 *
 * PURPOSE:
 *   Verify project-workspace ownership and filesystem-backed project import.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/project/project.h"

int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceSnapshot before;
    UmiProjectWorkspaceSnapshot after;
    UmiProjectWorkspaceSnapshot after_second_import;
    UmiProjectWorkspaceImportRequest request = {0};
    UmiProjectWorkspaceImportSnapshot imported;
    UmiProjectWorkspaceValidationReport global_validation;
    UmiProjectWorkspaceValidationReport scoped_validation;
    UmiProjectDescriptorSnapshot broken_project = {0};
    UmiProjectTaskSnapshot broken_task = {0};
    char temp_directory[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char source_file[UMI_PATH_CAPACITY];

    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 1;
    if (umi_project_workspace_snapshot(workspace, &before) != UMI_STATUS_OK ||
        before.item_count != 0U) return 2;

    if (umi_fs_temp_directory(temp_directory, sizeof(temp_directory)) != UMI_STATUS_OK)
        return 3;
    if (umi_path_join(temp_directory, "umicom-b31-project-import",
                      root, sizeof(root)) != UMI_STATUS_OK) return 4;
    (void)umi_fs_remove_tree(root);
    if (umi_fs_make_directories(root) != UMI_STATUS_OK) return 5;
    if (umi_path_join(root, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK) return 6;
    if (umi_path_join(root, "main.c", source_file,
                      sizeof(source_file)) != UMI_STATUS_OK) return 7;
    if (umi_fs_write_text(cmake_file,
            "cmake_minimum_required(VERSION 3.24)\nproject(sample C)\n") !=
        UMI_STATUS_OK) return 8;
    if (umi_fs_write_text(source_file,
            "int main(void) { return 0; }\n") != UMI_STATUS_OK) return 9;

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.root_directory = root;
    request.project_id = "sample";
    request.display_name = "Sample Project";
    request.build_directory = "out";
    request.configuration_name = "Debug";
    request.generator = "Ninja";
    request.parallel_jobs = 2U;
    request.create_test_task = 1;
    request.launch_program = "sample-app";

    if (umi_project_workspace_import_directory(
            workspace, &request, &imported) != UMI_STATUS_OK) return 10;
    if (!imported.has_cmake || !imported.has_launch_profile ||
        !imported.created_test_task || imported.c_source_count != 1U ||
        strcmp(imported.project_id, "sample") != 0) return 11;
    if (!imported.selection.has_configuration ||
        !imported.selection.has_target ||
        !imported.selection.has_task ||
        !imported.selection.has_environment ||
        !imported.selection.has_launch_profile ||
        !imported.validation.valid) return 12;

    if (umi_project_workspace_snapshot(workspace, &after) != UMI_STATUS_OK)
        return 13;
    if (after.descriptor_count != 1U || after.configuration_count != 1U ||
        after.target_count != 1U || after.environment_count != 1U ||
        after.file_set_count != 1U || after.task_count != 3U ||
        after.launch_profile_count != 1U || after.build_node_count != 4U)
        return 14;

    if (umi_project_workspace_import_directory(
            workspace, &request, &imported) != UMI_STATUS_OK) return 15;
    if (umi_project_workspace_snapshot(
            workspace, &after_second_import) != UMI_STATUS_OK) return 16;
    if (after_second_import.descriptor_count != after.descriptor_count ||
        after_second_import.task_count != after.task_count ||
        after_second_import.build_node_count != after.build_node_count ||
        after_second_import.revision <= after.revision) return 17;

    /*
     * A broken sibling project must not prevent the valid imported project
     * from passing project-scoped validation.
     */
    broken_project.struct_size = (uint32_t)sizeof(broken_project);
    broken_project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(broken_project.id, "broken");
    strcpy(broken_project.name, "Broken Project");
    broken_project.enabled = 1;
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace),
            &broken_project) != UMI_STATUS_OK) return 18;
    broken_task.struct_size = (uint32_t)sizeof(broken_task);
    broken_task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(broken_task.id, "broken.task");
    strcpy(broken_task.project_id, "broken");
    broken_task.enabled = 1;
    if (umi_project_task_registry_upsert(
            umi_project_workspace_task(workspace),
            &broken_task) != UMI_STATUS_OK) return 19;
    if (umi_project_workspace_validate(
            workspace, &global_validation) != UMI_STATUS_OK ||
        global_validation.valid != 0) return 20;
    if (umi_project_workspace_validate_project(
            workspace, "sample", &scoped_validation) != UMI_STATUS_OK ||
        scoped_validation.valid == 0) return 21;

    umi_project_workspace_destroy(workspace);
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 22;
    return 0;
}
