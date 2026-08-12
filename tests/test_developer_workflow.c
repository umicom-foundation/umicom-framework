/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_workflow.c
 *
 * PURPOSE:
 *   Verify named workflows chain project tasks and launch profiles in order and
 *   verify filesystem bootstrap can prepare a reusable project workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/developer.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

#include <assert.h>
#include <string.h>

static void add_task(UmiDeveloperRuntime *runtime, const char *id, const char *command)
{
    UmiProjectTaskSnapshot task = {0};
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.project_id, "studio");
    strcpy(task.group, "build");
    strcpy(task.label, id);
    strcpy(task.id, id);
    strcpy(task.command, command);
    task.enabled = 1;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)), &task)
        == UMI_STATUS_OK);
}

static void test_named_workflow(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiDeveloperWorkflowRequest request = {0};
    UmiDeveloperWorkflowSnapshot workflow;
    UmiDeveloperPipelineSnapshot pipeline;
    const char *tasks[] = {"configure", "build", "test"};

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "studio");
    strcpy(project.name, "Studio");
    project.enabled = 1;
    assert(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(umi_developer_runtime_projects(runtime)),
        &project) == UMI_STATUS_OK);
    add_task(runtime, "configure", "cmake -S . -B build");
    add_task(runtime, "build", "cmake --build build");
    add_task(runtime, "test", "ctest --test-dir build");

    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id, "run"); strcpy(profile.project_id, "studio");
    strcpy(profile.name, "Run"); strcpy(profile.program, "umicom-studio-console");
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)),
        &profile) == UMI_STATUS_OK);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_WORKFLOW_API_VERSION;
    request.workflow_id = "studio.verify";
    request.task_ids = tasks;
    request.task_count = 3U;
    request.launch_profile_id = "run";
    request.debug_mode = 0;
    assert(umi_developer_workflow_submit(runtime, &request, &workflow) == UMI_STATUS_OK);
    assert(workflow.operation_count == 4U);
    assert(workflow.dependency_count == 3U);
    assert(umi_developer_pipeline_snapshot(
        umi_developer_runtime_pipeline(runtime), &pipeline) == UMI_STATUS_OK);
    assert(pipeline.operation_count == 4U);
    assert(pipeline.dependency_count == 3U);
    assert(pipeline.ready_count == 1U);

    umi_developer_runtime_destroy(runtime);
}

static void test_project_bootstrap(void)
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
}

int main(void)
{
    test_named_workflow();
    test_project_bootstrap();
    return 0;
}
