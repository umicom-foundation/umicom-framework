/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_project_bootstrap.c
 *
 * PURPOSE:
 *   Verify filesystem project bootstrap prepares metadata, developer context
 *   and a dependency-aware configure/build/test workflow.
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

#include "umicom/developer/developer.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiDeveloperProjectBootstrapRequest request = {0};
    UmiDeveloperProjectBootstrapSnapshot snapshot;
    UmiDeveloperPipelineSnapshot pipeline;
    char temp_directory[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char source_file[UMI_PATH_CAPACITY];

    assert(umi_fs_temp_directory(temp_directory, sizeof(temp_directory)) == UMI_STATUS_OK);
    assert(umi_path_join(temp_directory, "umicom-b31-bootstrap-test",
                         root, sizeof(root)) == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(root);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "CMakeLists.txt", cmake_file,
                         sizeof(cmake_file)) == UMI_STATUS_OK);
    assert(umi_path_join(root, "main.c", source_file,
                         sizeof(source_file)) == UMI_STATUS_OK);
    assert(umi_fs_write_text(cmake_file,
        "cmake_minimum_required(VERSION 3.24)\nproject(bootstrap C)\n") == UMI_STATUS_OK);
    assert(umi_fs_write_text(source_file,
        "int main(void) { return 0; }\n") == UMI_STATUS_OK);

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;
    request.project.struct_size = (uint32_t)sizeof(request.project);
    request.project.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.project.root_directory = root;
    request.project.project_id = "bootstrap";
    request.project.create_test_task = 1;
    request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_TEST;
    request.workflow_id = "bootstrap.verify";
    request.prepare_workflow = 1;
    request.include_configure = 1;

    assert(umi_developer_project_bootstrap(runtime, &request, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.workflow_prepared == 1);
    assert(snapshot.project.validation.valid == 1);
    assert(snapshot.workflow.workflow.operation_count == 3U);
    assert(strcmp(snapshot.context.project_id, "bootstrap") == 0);
    assert(umi_developer_pipeline_snapshot(
        umi_developer_runtime_pipeline(runtime), &pipeline) == UMI_STATUS_OK);
    assert(pipeline.operation_count == 3U);
    assert(pipeline.dependency_count == 2U);
    assert(pipeline.ready_count == 1U);

    umi_developer_runtime_destroy(runtime);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
