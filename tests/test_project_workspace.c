/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "umicom/project/project.h"

int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceSnapshot before;
    UmiProjectWorkspaceSnapshot after;
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectConfigurationSnapshot configuration = {0};
    UmiProjectTargetSnapshot target = {0};
    UmiProjectTaskSnapshot task = {0};
    UmiProjectLaunchProfileSnapshot launch = {0};
    UmiProjectEnvironmentSnapshot environment = {0};
    UmiProjectDependencySnapshot dependency = {0};
    UmiProjectWorkspaceSelectionRequest request = {0};
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;

    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 1;
    if (umi_project_workspace_snapshot(workspace, &before) != UMI_STATUS_OK ||
        before.item_count != 0U) return 2;

    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "studio");
    strcpy(project.name, "Umicom Studio");
    strcpy(project.root_uri, ".");
    strcpy(project.kind, "application");
    strcpy(project.primary_language, "C");
    project.enabled = 1;
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace), &project) != UMI_STATUS_OK) return 3;

    configuration.struct_size = (uint32_t)sizeof(configuration);
    configuration.api_version = UMI_PROJECT_CONFIGURATION_API_VERSION;
    strcpy(configuration.id, "debug");
    strcpy(configuration.project_id, "studio");
    strcpy(configuration.name, "Debug");
    strcpy(configuration.build_type, "Debug");
    strcpy(configuration.toolchain_id, "clang-ucrt64");
    configuration.active = 1;
    if (umi_project_configuration_registry_upsert(
            umi_project_workspace_configuration(workspace),
            &configuration) != UMI_STATUS_OK) return 4;

    target.struct_size = (uint32_t)sizeof(target);
    target.api_version = UMI_PROJECT_TARGET_API_VERSION;
    strcpy(target.id, "studio-console");
    strcpy(target.project_id, "studio");
    strcpy(target.name, "Studio Console");
    strcpy(target.kind, "executable");
    target.enabled = 1;
    target.default_target = 1;
    if (umi_project_target_registry_upsert(
            umi_project_workspace_target(workspace), &target) != UMI_STATUS_OK) return 5;

    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.id, "build");
    strcpy(task.project_id, "studio");
    strcpy(task.label, "Build");
    strcpy(task.command, "cmake --build build");
    strcpy(task.group, "build");
    task.enabled = 1;
    task.default_task = 1;
    if (umi_project_task_registry_upsert(
            umi_project_workspace_task(workspace), &task) != UMI_STATUS_OK) return 6;

    launch.struct_size = (uint32_t)sizeof(launch);
    launch.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(launch.id, "run");
    strcpy(launch.project_id, "studio");
    strcpy(launch.name, "Run Studio");
    strcpy(launch.program, "umicom-studio-console");
    strcpy(launch.environment_id, "local");
    launch.default_profile = 1;
    if (umi_project_launch_profile_registry_upsert(
            umi_project_workspace_launch_profile(workspace),
            &launch) != UMI_STATUS_OK) return 7;

    environment.struct_size = (uint32_t)sizeof(environment);
    environment.api_version = UMI_PROJECT_ENVIRONMENT_API_VERSION;
    strcpy(environment.id, "local");
    strcpy(environment.project_id, "studio");
    strcpy(environment.name, "Local UCRT64");
    strcpy(environment.toolchain_id, "clang-ucrt64");
    environment.inherit_parent = 1;
    if (umi_project_environment_registry_upsert(
            umi_project_workspace_environment(workspace),
            &environment) != UMI_STATUS_OK) return 8;

    dependency.struct_size = (uint32_t)sizeof(dependency);
    dependency.api_version = UMI_PROJECT_DEPENDENCY_API_VERSION;
    strcpy(dependency.id, "framework");
    strcpy(dependency.project_id, "studio");
    strcpy(dependency.name, "Umicom Framework");
    dependency.optional = 0;
    dependency.resolved = 0;
    if (umi_project_dependency_registry_upsert(
            umi_project_workspace_dependency(workspace),
            &dependency) != UMI_STATUS_OK) return 9;

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    request.project_id = "studio";
    if (umi_project_workspace_resolve_selection(
            workspace, &request, &selection) != UMI_STATUS_OK) return 10;
    if (!selection.has_configuration || !selection.has_target ||
        !selection.has_task || !selection.has_launch_profile ||
        !selection.has_environment) return 11;
    if (strcmp(selection.configuration.id, "debug") != 0 ||
        strcmp(selection.target.id, "studio-console") != 0 ||
        strcmp(selection.task.id, "build") != 0 ||
        strcmp(selection.launch_profile.id, "run") != 0 ||
        strcmp(selection.environment.id, "local") != 0) return 12;
    if (selection.unresolved_required_dependency_count != 1U) return 13;

    if (umi_project_workspace_validate(workspace, &validation) != UMI_STATUS_OK) return 14;
    if (validation.valid != 0 ||
        validation.unresolved_required_dependency_count != 1U ||
        validation.error_count == 0U) return 15;

    dependency.resolved = 1;
    if (umi_project_dependency_registry_upsert(
            umi_project_workspace_dependency(workspace),
            &dependency) != UMI_STATUS_OK) return 16;
    if (umi_project_workspace_validate(workspace, &validation) != UMI_STATUS_OK ||
        validation.valid == 0 ||
        validation.unresolved_required_dependency_count != 0U) return 17;

    if (umi_project_workspace_snapshot(workspace, &after) != UMI_STATUS_OK) return 18;
    if (after.revision <= before.revision || after.item_count != 7U) return 19;

    umi_project_workspace_destroy(workspace);
    return 0;
}
