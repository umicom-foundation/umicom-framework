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

/*
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

/**
 * Represent the project workspace selection request data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the project workspace selection snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Provide the project workspace resolve selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_resolve_selection(
    const UmiProjectWorkspace *workspace,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

/**
 * Provide the project workspace find active configuration operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_find_active_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectConfigurationSnapshot *out_configuration);

/**
 * Provide the project workspace find default target operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_default_target(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTargetSnapshot *out_target);

/**
 * Provide the project workspace find default task operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_default_task(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTaskSnapshot *out_task);

/**
 * Provide the project workspace find task by group operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_task_by_group(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const char *group,
    UmiProjectTaskSnapshot *out_task);

/**
 * Provide the project workspace find default launch profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_find_default_launch_profile(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectLaunchProfileSnapshot *out_profile);

/**
 * Provide the project workspace find environment for configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_find_environment_for_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const UmiProjectConfigurationSnapshot *configuration,
    UmiProjectEnvironmentSnapshot *out_environment);

/**
 * Provide the project workspace configuration count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_configuration_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Provide the project workspace target count for project operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_target_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Provide the project workspace task count for project operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_task_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Provide the project workspace launch profile count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_launch_profile_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Provide the project workspace environment count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_environment_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Provide the project workspace dependency count for project operation used by this module
 * and its client applications.
 */
size_t umi_project_workspace_dependency_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id);
/**
 * Return the number of records represented by project workspace unresolved required
 * dependency without changing their state.
 */
size_t umi_project_workspace_unresolved_required_dependency_count(
    const UmiProjectWorkspace *workspace, const char *project_id);

#ifdef __cplusplus
}
#endif
#endif
