/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_query.h
 *
 * PURPOSE:
 *   Resolve the active/default project configuration, target, task, launch
 *   profile and environment from the reusable project workspace without
 *   exposing registry internals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A project can contain many build configurations and launch profiles. Product
 * code should not reimplement "pick the active/default item" rules. This
 * contract centralises those rules so Studio, CLI tools and future applications
 * make the same deterministic choice.
 */
#ifndef UMICOM_PROJECT_WORKSPACE_QUERY_H
#define UMICOM_PROJECT_WORKSPACE_QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/project/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_QUERY_API_VERSION 1U

typedef struct UmiProjectWorkspaceSelectionRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *project_id;
    const char *configuration_id;
    const char *target_id;
    const char *task_id;
    const char *launch_profile_id;
    const char *environment_id;
} UmiProjectWorkspaceSelectionRequest;

typedef struct UmiProjectWorkspaceSelectionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectDescriptorSnapshot project;
    UmiProjectConfigurationSnapshot configuration;
    UmiProjectTargetSnapshot target;
    UmiProjectTaskSnapshot task;
    UmiProjectLaunchProfileSnapshot launch_profile;
    UmiProjectEnvironmentSnapshot environment;
    size_t project_count;
    size_t configuration_count;
    size_t target_count;
    size_t task_count;
    size_t launch_profile_count;
    size_t environment_count;
    size_t dependency_count;
    size_t unresolved_required_dependency_count;
    int has_configuration;
    int has_target;
    int has_task;
    int has_launch_profile;
    int has_environment;
} UmiProjectWorkspaceSelectionSnapshot;

UmiStatus umi_project_workspace_resolve_selection(
    const UmiProjectWorkspace *workspace,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_project_workspace_find_active_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectConfigurationSnapshot *out_configuration);

UmiStatus umi_project_workspace_find_default_target(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTargetSnapshot *out_target);

UmiStatus umi_project_workspace_find_default_task(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTaskSnapshot *out_task);

UmiStatus umi_project_workspace_find_task_by_group(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const char *group,
    UmiProjectTaskSnapshot *out_task);

UmiStatus umi_project_workspace_find_default_launch_profile(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectLaunchProfileSnapshot *out_profile);

UmiStatus umi_project_workspace_find_environment_for_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const UmiProjectConfigurationSnapshot *configuration,
    UmiProjectEnvironmentSnapshot *out_environment);

size_t umi_project_workspace_configuration_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_target_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_task_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_launch_profile_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_environment_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_dependency_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
size_t umi_project_workspace_unresolved_required_dependency_count(
    const UmiProjectWorkspace *workspace, const char *project_id);

#ifdef __cplusplus
}
#endif
#endif
