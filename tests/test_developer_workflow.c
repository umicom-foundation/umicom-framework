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
#include "umicom/developer/project_workflow.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void add_task(UmiDeveloperRuntime *runtime,
                     const char *id,
                     const char *group,
                     const char *command,
                     int default_task)
{
    UmiProjectTaskSnapshot task = {0};
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.project_id, "studio");
    strcpy(task.group, group);
    strcpy(task.label, id);
    strcpy(task.id, id);
    strcpy(task.command, command);
    task.enabled = 1;
    task.default_task = default_task;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)), &task)
        == UMI_STATUS_OK);
}

static void add_project_model(UmiDeveloperRuntime *runtime)
{
    UmiProjectWorkspace *workspace = umi_developer_runtime_projects(runtime);
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectConfigurationSnapshot configuration = {0};
    UmiProjectTargetSnapshot target = {0};
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiProjectEnvironmentSnapshot environment = {0};

    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "studio");
    strcpy(project.name, "Umicom Studio");
    strcpy(project.root_uri, ".");
    project.enabled = 1;
    assert(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace), &project) == UMI_STATUS_OK);

    configuration.struct_size = (uint32_t)sizeof(configuration);
    configuration.api_version = UMI_PROJECT_CONFIGURATION_API_VERSION;
    strcpy(configuration.id, "debug");
    strcpy(configuration.project_id, "studio");
    strcpy(configuration.name, "Debug");
    strcpy(configuration.toolchain_id, "clang-ucrt64");
    configuration.active = 1;
    assert(umi_project_configuration_registry_upsert(
        umi_project_workspace_configuration(workspace), &configuration)
        == UMI_STATUS_OK);

    target.struct_size = (uint32_t)sizeof(target);
    target.api_version = UMI_PROJECT_TARGET_API_VERSION;
    strcpy(target.id, "studio-console");
    strcpy(target.project_id, "studio");
    strcpy(target.name, "Studio Console");
    target.enabled = 1;
    target.default_target = 1;
    assert(umi_project_target_registry_upsert(
        umi_project_workspace_target(workspace), &target) == UMI_STATUS_OK);

    environment.struct_size = (uint32_t)sizeof(environment);
    environment.api_version = UMI_PROJECT_ENVIRONMENT_API_VERSION;
    strcpy(environment.id, "local");
    strcpy(environment.project_id, "studio");
    strcpy(environment.name, "Local");
    strcpy(environment.toolchain_id, "clang-ucrt64");
    environment.inherit_parent = 1;
    assert(umi_project_environment_registry_upsert(
        umi_project_workspace_environment(workspace), &environment)
        == UMI_STATUS_OK);

    add_task(runtime, "configure", "configure", "cmake -S . -B build", 0);
    add_task(runtime, "build", "build", "cmake --build build", 1);
    add_task(runtime, "test", "test", "ctest --test-dir build", 0);

    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id, "run");
    strcpy(profile.project_id, "studio");
    strcpy(profile.name, "Run");
    strcpy(profile.program, "umicom-studio-console");
    strcpy(profile.environment_id, "local");
    profile.default_profile = 1;
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(workspace), &profile) == UMI_STATUS_OK);
}

int main(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiDeveloperWorkflowRequest request = {0};
    UmiDeveloperWorkflowSnapshot workflow;
    UmiDeveloperProjectWorkflowRequest project_request = {0};
    UmiDeveloperProjectWorkflowSnapshot project_workflow;
    UmiDeveloperPipelineSnapshot pipeline;
    UmiDeveloperContextSnapshot context;
    const char *tasks[] = {"configure", "build", "test"};

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    add_project_model(runtime);

    /* Existing explicit workflow API remains supported. */
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_WORKFLOW_API_VERSION;
    request.workflow_id = "studio.explicit";
    request.task_ids = tasks;
    request.task_count = 3U;
    request.launch_profile_id = "run";
    request.debug_mode = 0;
    assert(umi_developer_workflow_submit(runtime, &request, &workflow) == UMI_STATUS_OK);
    assert(workflow.operation_count == 4U);
    assert(workflow.dependency_count == 3U);

    /*
     * Use a fresh runtime for the high-level preset so operation identifiers
     * from the explicit test do not influence the result.
     */
    umi_developer_runtime_destroy(runtime);
    runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    add_project_model(runtime);

    project_request.struct_size = (uint32_t)sizeof(project_request);
    project_request.api_version = UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    project_request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_FULL;
    project_request.workflow_id = "studio.full";
    project_request.project_id = "studio";
    project_request.include_configure = 1;
    project_request.max_attempts = 2U;

    assert(umi_developer_project_workflow_submit(
        runtime, &project_request, &project_workflow) == UMI_STATUS_OK);
    assert(project_workflow.validation.valid != 0);
    assert(project_workflow.task_count == 3U);
    assert(project_workflow.uses_launch_profile != 0);
    assert(project_workflow.workflow.operation_count == 4U);
    assert(project_workflow.workflow.dependency_count == 3U);
    assert(strcmp(project_workflow.selection.project.id, "studio") == 0);
    assert(strcmp(project_workflow.selection.configuration.id, "debug") == 0);
    assert(strcmp(project_workflow.selection.target.id, "studio-console") == 0);

    assert(umi_developer_context_snapshot(
        umi_developer_runtime_context(runtime), &context) == UMI_STATUS_OK);
    assert(strcmp(context.project_id, "studio") == 0);
    assert(strcmp(context.configuration_id, "debug") == 0);
    assert(strcmp(context.target_id, "studio-console") == 0);
    assert(strcmp(context.launch_profile_id, "run") == 0);

    assert(umi_developer_pipeline_snapshot(
        umi_developer_runtime_pipeline(runtime), &pipeline) == UMI_STATUS_OK);
    assert(pipeline.operation_count == 4U);
    assert(pipeline.dependency_count == 3U);
    assert(pipeline.ready_count == 1U);

    umi_developer_runtime_destroy(runtime);
    return 0;
}
