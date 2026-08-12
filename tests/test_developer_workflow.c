/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_workflow.c
 *
 * PURPOSE:
 *   Verify named workflows chain project tasks and launch profiles in order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/workflow.h"

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

int main(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiDeveloperWorkflowRequest request = {0};
    UmiDeveloperWorkflowSnapshot workflow;
    UmiDeveloperPipelineSnapshot pipeline;
    const char *tasks[] = {"configure", "build", "test"};

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
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
    return 0;
}
