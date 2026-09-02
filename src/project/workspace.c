/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace.c
 *
 * PURPOSE:
 *   Implement the reusable project workspace that owns project metadata, targets, dependencies, tasks, launch profiles and build graph records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/project/workspace.h"
#include "umicom/project/workspace_query.h"
#include "umicom/project/workspace_validation.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectWorkspace {
    UmiProjectDescriptorRegistry *descriptor;
    UmiProjectTargetRegistry *target;
    UmiProjectConfigurationRegistry *configuration;
    UmiProjectDependencyRegistry *dependency;
    UmiProjectFileSetRegistry *file_set;
    UmiProjectTaskRegistry *task;
    UmiProjectLaunchProfileRegistry *launch_profile;
    UmiProjectEnvironmentRegistry *environment;
    UmiProjectReferenceRegistry *reference;
    UmiProjectTemplateRegistry *template;
    UmiProjectVariableRegistry *variable;
    UmiProjectCapabilityRegistry *capability;
    UmiProjectBuildNodeRegistry *build_node;
    uint64_t revision;
};

/*
 * Initialise project workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_project_workspace_create(UmiProjectWorkspace **out_owner)
{
    UmiProjectWorkspace *owner; UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiProjectWorkspace *)calloc(1U,sizeof(*owner));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_descriptor_registry_create(&owner->descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_target_registry_create(&owner->target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_configuration_registry_create(&owner->configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_dependency_registry_create(&owner->dependency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_file_set_registry_create(&owner->file_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_task_registry_create(&owner->task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_launch_profile_registry_create(&owner->launch_profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_environment_registry_create(&owner->environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_reference_registry_create(&owner->reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_template_registry_create(&owner->template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_variable_registry_create(&owner->variable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_capability_registry_create(&owner->capability);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_project_build_node_registry_create(&owner->build_node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { umi_project_workspace_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

/*
 * Release or reset state held by project workspace so the same storage can be reused
 * safely.
 */
void umi_project_workspace_destroy(UmiProjectWorkspace *owner)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return;
    umi_project_build_node_registry_destroy(owner->build_node);
    umi_project_capability_registry_destroy(owner->capability);
    umi_project_variable_registry_destroy(owner->variable);
    umi_project_template_registry_destroy(owner->template);
    umi_project_reference_registry_destroy(owner->reference);
    umi_project_environment_registry_destroy(owner->environment);
    umi_project_launch_profile_registry_destroy(owner->launch_profile);
    umi_project_task_registry_destroy(owner->task);
    umi_project_file_set_registry_destroy(owner->file_set);
    umi_project_dependency_registry_destroy(owner->dependency);
    umi_project_configuration_registry_destroy(owner->configuration);
    umi_project_target_registry_destroy(owner->target);
    umi_project_descriptor_registry_destroy(owner->descriptor);
    free(owner);
}

/*
 * Provide the workspace revision operation used by this module and its client
 * applications.
 */
static uint64_t workspace_revision(const UmiProjectWorkspace *owner)
{
    uint64_t revision;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return 0U;
    revision = owner->revision;
    revision += umi_project_descriptor_registry_revision(owner->descriptor);
    revision += umi_project_target_registry_revision(owner->target);
    revision += umi_project_configuration_registry_revision(owner->configuration);
    revision += umi_project_dependency_registry_revision(owner->dependency);
    revision += umi_project_file_set_registry_revision(owner->file_set);
    revision += umi_project_task_registry_revision(owner->task);
    revision += umi_project_launch_profile_registry_revision(owner->launch_profile);
    revision += umi_project_environment_registry_revision(owner->environment);
    revision += umi_project_reference_registry_revision(owner->reference);
    revision += umi_project_template_registry_revision(owner->template);
    revision += umi_project_variable_registry_revision(owner->variable);
    revision += umi_project_capability_registry_revision(owner->capability);
    revision += umi_project_build_node_registry_revision(owner->build_node);
    return revision;
}

/*
 * Provide the project workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_workspace_snapshot(const UmiProjectWorkspace *owner, UmiProjectWorkspaceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=workspace_revision(owner);
    out_snapshot->descriptor_count = umi_project_descriptor_registry_count(owner->descriptor);
    out_snapshot->target_count = umi_project_target_registry_count(owner->target);
    out_snapshot->configuration_count = umi_project_configuration_registry_count(owner->configuration);
    out_snapshot->dependency_count = umi_project_dependency_registry_count(owner->dependency);
    out_snapshot->file_set_count = umi_project_file_set_registry_count(owner->file_set);
    out_snapshot->task_count = umi_project_task_registry_count(owner->task);
    out_snapshot->launch_profile_count = umi_project_launch_profile_registry_count(owner->launch_profile);
    out_snapshot->environment_count = umi_project_environment_registry_count(owner->environment);
    out_snapshot->reference_count = umi_project_reference_registry_count(owner->reference);
    out_snapshot->template_count = umi_project_template_registry_count(owner->template);
    out_snapshot->variable_count = umi_project_variable_registry_count(owner->variable);
    out_snapshot->capability_count = umi_project_capability_registry_count(owner->capability);
    out_snapshot->build_node_count = umi_project_build_node_registry_count(owner->build_node);
    out_snapshot->item_count = out_snapshot->descriptor_count + out_snapshot->target_count + out_snapshot->configuration_count + out_snapshot->dependency_count + out_snapshot->file_set_count + out_snapshot->task_count + out_snapshot->launch_profile_count + out_snapshot->environment_count + out_snapshot->reference_count + out_snapshot->template_count + out_snapshot->variable_count + out_snapshot->capability_count + out_snapshot->build_node_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace descriptor operation used by this module and its client
 * applications.
 */
UmiProjectDescriptorRegistry *umi_project_workspace_descriptor(UmiProjectWorkspace *owner) { return owner != NULL ? owner->descriptor : NULL; }
/*
 * Provide the project workspace target operation used by this module and its client
 * applications.
 */
UmiProjectTargetRegistry *umi_project_workspace_target(UmiProjectWorkspace *owner) { return owner != NULL ? owner->target : NULL; }
/*
 * Provide the project workspace configuration operation used by this module and its client
 * applications.
 */
UmiProjectConfigurationRegistry *umi_project_workspace_configuration(UmiProjectWorkspace *owner) { return owner != NULL ? owner->configuration : NULL; }
/*
 * Provide the project workspace dependency operation used by this module and its client
 * applications.
 */
UmiProjectDependencyRegistry *umi_project_workspace_dependency(UmiProjectWorkspace *owner) { return owner != NULL ? owner->dependency : NULL; }
/*
 * Copy project workspace file into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiProjectFileSetRegistry *umi_project_workspace_file_set(UmiProjectWorkspace *owner) { return owner != NULL ? owner->file_set : NULL; }
/*
 * Provide the project workspace task operation used by this module and its client
 * applications.
 */
UmiProjectTaskRegistry *umi_project_workspace_task(UmiProjectWorkspace *owner) { return owner != NULL ? owner->task : NULL; }
/*
 * Provide the project workspace launch profile operation used by this module and its
 * client applications.
 */
UmiProjectLaunchProfileRegistry *umi_project_workspace_launch_profile(UmiProjectWorkspace *owner) { return owner != NULL ? owner->launch_profile : NULL; }
/*
 * Provide the project workspace environment operation used by this module and its client
 * applications.
 */
UmiProjectEnvironmentRegistry *umi_project_workspace_environment(UmiProjectWorkspace *owner) { return owner != NULL ? owner->environment : NULL; }
/*
 * Provide the project workspace reference operation used by this module and its client
 * applications.
 */
UmiProjectReferenceRegistry *umi_project_workspace_reference(UmiProjectWorkspace *owner) { return owner != NULL ? owner->reference : NULL; }
/*
 * Provide the project workspace template operation used by this module and its client
 * applications.
 */
UmiProjectTemplateRegistry *umi_project_workspace_template(UmiProjectWorkspace *owner) { return owner != NULL ? owner->template : NULL; }
/*
 * Provide the project workspace variable operation used by this module and its client
 * applications.
 */
UmiProjectVariableRegistry *umi_project_workspace_variable(UmiProjectWorkspace *owner) { return owner != NULL ? owner->variable : NULL; }
/*
 * Provide the project workspace capability operation used by this module and its client
 * applications.
 */
UmiProjectCapabilityRegistry *umi_project_workspace_capability(UmiProjectWorkspace *owner) { return owner != NULL ? owner->capability : NULL; }
/*
 * Provide the project workspace build node operation used by this module and its client
 * applications.
 */
UmiProjectBuildNodeRegistry *umi_project_workspace_build_node(UmiProjectWorkspace *owner) { return owner != NULL ? owner->build_node : NULL; }

/* ------------------------------------------------------------------------- */
/* Project-scoped query and deterministic selection support.                  */
/* ------------------------------------------------------------------------- */

static int same_project(const char *item_project_id, const char *project_id)
{
    return item_project_id != NULL && project_id != NULL &&
           strcmp(item_project_id, project_id) == 0;
}

/* Provide the resolve project operation used by this module and its client applications. */
static UmiStatus resolve_project(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectDescriptorSnapshot *out_project)
{
    size_t index;
    UmiProjectDescriptorSnapshot candidate;
    UmiProjectDescriptorSnapshot fallback;
    int have_fallback = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_project == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_id != NULL && project_id[0] != '\0') {
        return umi_project_descriptor_registry_find(
            workspace->descriptor, project_id, out_project);
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_project_descriptor_registry_count(workspace->descriptor);
         ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_project_descriptor_registry_at(
                workspace->descriptor, index, &candidate) != UMI_STATUS_OK) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) {
            fallback = candidate;
            have_fallback = 1;
        }
        /* Apply this operation only while the related capability or state is available. */
        if (candidate.enabled != 0) {
            *out_project = candidate;
            return UMI_STATUS_OK;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (have_fallback) {
        *out_project = fallback;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace configuration count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_configuration_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectConfigurationSnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_configuration_registry_count(workspace->configuration); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_configuration_registry_at(workspace->configuration,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Provide the project workspace target count for project operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_target_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectTargetSnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_target_registry_count(workspace->target); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_target_registry_at(workspace->target,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Provide the project workspace task count for project operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_task_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectTaskSnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_task_registry_count(workspace->task); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_task_registry_at(workspace->task,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Provide the project workspace launch profile count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_launch_profile_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectLaunchProfileSnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_launch_profile_registry_count(workspace->launch_profile); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_launch_profile_registry_at(workspace->launch_profile,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Provide the project workspace environment count for project operation used by this
 * module and its client applications.
 */
size_t umi_project_workspace_environment_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectEnvironmentSnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_environment_registry_count(workspace->environment); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_environment_registry_at(workspace->environment,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Provide the project workspace dependency count for project operation used by this module
 * and its client applications.
 */
size_t umi_project_workspace_dependency_count_for_project(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectDependencySnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_dependency_registry_count(workspace->dependency); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_dependency_registry_at(workspace->dependency,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id)) ++count;
    }
    return count;
}

/*
 * Return the number of records represented by project workspace unresolved required
 * dependency without changing their state.
 */
size_t umi_project_workspace_unresolved_required_dependency_count(
    const UmiProjectWorkspace *workspace, const char *project_id)
{
    size_t index, count = 0U;
    UmiProjectDependencySnapshot item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_dependency_registry_count(workspace->dependency); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_dependency_registry_at(workspace->dependency,index,&item)==UMI_STATUS_OK &&
            same_project(item.project_id,project_id) &&
            item.optional == 0 && item.resolved == 0) ++count;
    }
    return count;
}

/*
 * Provide the project workspace find active configuration operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_find_active_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectConfigurationSnapshot *out_configuration)
{
    size_t index;
    UmiProjectConfigurationSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || out_configuration == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_configuration_registry_count(workspace->configuration); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_configuration_registry_at(workspace->configuration,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item.active != 0) { *out_configuration=item; return UMI_STATUS_OK; }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_configuration=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace find default target operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_default_target(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTargetSnapshot *out_target)
{
    size_t index;
    UmiProjectTargetSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || out_target == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_target_registry_count(workspace->target); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_target_registry_at(workspace->target,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id) || item.enabled==0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item.default_target != 0) { *out_target=item; return UMI_STATUS_OK; }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_target=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace find default task operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_default_task(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectTaskSnapshot *out_task)
{
    size_t index;
    UmiProjectTaskSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || out_task == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_task_registry_count(workspace->task); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_task_registry_at(workspace->task,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id) || item.enabled==0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item.default_task != 0) { *out_task=item; return UMI_STATUS_OK; }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_task=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace find task by group operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_find_task_by_group(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const char *group,
    UmiProjectTaskSnapshot *out_task)
{
    size_t index;
    UmiProjectTaskSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || group == NULL ||
        group[0]=='\0' || out_task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_task_registry_count(workspace->task); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_task_registry_at(workspace->task,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id) || item.enabled==0 ||
            strcmp(item.group,group)!=0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item.default_task != 0) { *out_task=item; return UMI_STATUS_OK; }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_task=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace find default launch profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_find_default_launch_profile(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectLaunchProfileSnapshot *out_profile)
{
    size_t index;
    UmiProjectLaunchProfileSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_launch_profile_registry_count(workspace->launch_profile); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_launch_profile_registry_at(workspace->launch_profile,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item.default_profile != 0) { *out_profile=item; return UMI_STATUS_OK; }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_profile=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the project workspace find environment for configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_find_environment_for_configuration(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    const UmiProjectConfigurationSnapshot *configuration,
    UmiProjectEnvironmentSnapshot *out_environment)
{
    size_t index;
    UmiProjectEnvironmentSnapshot item, fallback;
    int have_fallback = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || project_id == NULL || out_environment == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_project_environment_registry_count(workspace->environment); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_environment_registry_at(workspace->environment,index,&item)!=UMI_STATUS_OK ||
            !same_project(item.project_id,project_id)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!have_fallback) { fallback=item; have_fallback=1; }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (configuration != NULL && configuration->toolchain_id[0] != '\0' &&
            strcmp(configuration->toolchain_id,item.toolchain_id)==0) {
            *out_environment=item; return UMI_STATUS_OK;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (have_fallback) { *out_environment=fallback; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the resolve requested configuration operation used by this module and its client
 * applications.
 */
static UmiStatus resolve_requested_configuration(
    const UmiProjectWorkspace *workspace, const char *project_id,
    const char *id, UmiProjectConfigurationSnapshot *out)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL || id[0]=='\0')
        return umi_project_workspace_find_active_configuration(workspace,project_id,out);
    status=umi_project_configuration_registry_find(workspace->configuration,id,out);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    return same_project(out->project_id,project_id)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the resolve requested target operation used by this module and its client
 * applications.
 */
static UmiStatus resolve_requested_target(
    const UmiProjectWorkspace *workspace, const char *project_id,
    const char *id, UmiProjectTargetSnapshot *out)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL || id[0]=='\0')
        return umi_project_workspace_find_default_target(workspace,project_id,out);
    status=umi_project_target_registry_find(workspace->target,id,out);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    return same_project(out->project_id,project_id)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the resolve requested task operation used by this module and its client
 * applications.
 */
static UmiStatus resolve_requested_task(
    const UmiProjectWorkspace *workspace, const char *project_id,
    const char *id, UmiProjectTaskSnapshot *out)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL || id[0]=='\0')
        return umi_project_workspace_find_default_task(workspace,project_id,out);
    status=umi_project_task_registry_find(workspace->task,id,out);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    return same_project(out->project_id,project_id)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the resolve requested launch operation used by this module and its client
 * applications.
 */
static UmiStatus resolve_requested_launch(
    const UmiProjectWorkspace *workspace, const char *project_id,
    const char *id, UmiProjectLaunchProfileSnapshot *out)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL || id[0]=='\0')
        return umi_project_workspace_find_default_launch_profile(workspace,project_id,out);
    status=umi_project_launch_profile_registry_find(workspace->launch_profile,id,out);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    return same_project(out->project_id,project_id)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the resolve requested environment operation used by this module and its client
 * applications.
 */
static UmiStatus resolve_requested_environment(
    const UmiProjectWorkspace *workspace, const char *project_id,
    const char *id, const UmiProjectConfigurationSnapshot *configuration,
    UmiProjectEnvironmentSnapshot *out)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL || id[0]=='\0')
        return umi_project_workspace_find_environment_for_configuration(
            workspace,project_id,configuration,out);
    status=umi_project_environment_registry_find(workspace->environment,id,out);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    return same_project(out->project_id,project_id)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the project workspace resolve selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_resolve_selection(
    const UmiProjectWorkspace *workspace,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection)
{
    const char *project_id = NULL;
    const char *configuration_id = NULL;
    const char *target_id = NULL;
    const char *task_id = NULL;
    const char *launch_profile_id = NULL;
    const char *environment_id = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_selection == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL) {
        project_id=request->project_id;
        configuration_id=request->configuration_id;
        target_id=request->target_id;
        task_id=request->task_id;
        launch_profile_id=request->launch_profile_id;
        environment_id=request->environment_id;
    }

    memset(out_selection,0,sizeof(*out_selection));
    out_selection->struct_size=(uint32_t)sizeof(*out_selection);
    out_selection->api_version=UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    out_selection->project_count=umi_project_descriptor_registry_count(workspace->descriptor);

    status=resolve_project(workspace,project_id,&out_selection->project);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status!=UMI_STATUS_OK) return status;
    project_id=out_selection->project.id;

    out_selection->configuration_count=
        umi_project_workspace_configuration_count_for_project(workspace,project_id);
    out_selection->target_count=
        umi_project_workspace_target_count_for_project(workspace,project_id);
    out_selection->task_count=
        umi_project_workspace_task_count_for_project(workspace,project_id);
    out_selection->launch_profile_count=
        umi_project_workspace_launch_profile_count_for_project(workspace,project_id);
    out_selection->environment_count=
        umi_project_workspace_environment_count_for_project(workspace,project_id);
    out_selection->dependency_count=
        umi_project_workspace_dependency_count_for_project(workspace,project_id);
    out_selection->unresolved_required_dependency_count=
        umi_project_workspace_unresolved_required_dependency_count(workspace,project_id);

    status=resolve_requested_configuration(workspace,project_id,configuration_id,
                                           &out_selection->configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status==UMI_STATUS_OK) out_selection->has_configuration=1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status!=UMI_STATUS_NOT_FOUND) return status;

    status=resolve_requested_target(workspace,project_id,target_id,
                                    &out_selection->target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status==UMI_STATUS_OK) out_selection->has_target=1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status!=UMI_STATUS_NOT_FOUND) return status;

    status=resolve_requested_task(workspace,project_id,task_id,
                                  &out_selection->task);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status==UMI_STATUS_OK) out_selection->has_task=1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status!=UMI_STATUS_NOT_FOUND) return status;

    status=resolve_requested_launch(workspace,project_id,launch_profile_id,
                                    &out_selection->launch_profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status==UMI_STATUS_OK) out_selection->has_launch_profile=1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status!=UMI_STATUS_NOT_FOUND) return status;

    status=resolve_requested_environment(
        workspace,project_id,environment_id,
        out_selection->has_configuration ? &out_selection->configuration : NULL,
        &out_selection->environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status==UMI_STATUS_OK) out_selection->has_environment=1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status!=UMI_STATUS_NOT_FOUND) return status;

    return UMI_STATUS_OK;
}

/* ------------------------------------------------------------------------- */
/* Workspace structural validation.                                          */
/* ------------------------------------------------------------------------- */

static void copy_issue_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination==NULL || capacity==0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source==NULL) source="";
    length=strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length>=capacity) length=capacity-1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length>0U) memcpy(destination,source,length);
    destination[length]='\0';
}

/* Provide the add issue operation used by this module and its client applications. */
static void add_issue(
    UmiProjectWorkspaceValidationReport *report,
    UmiProjectWorkspaceIssueSeverity severity,
    const char *code,
    const char *project_id,
    const char *item_id,
    const char *message)
{
    UmiProjectWorkspaceValidationIssue *issue;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report==NULL) return;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (severity==UMI_PROJECT_WORKSPACE_ISSUE_ERROR) report->error_count+=1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (severity==UMI_PROJECT_WORKSPACE_ISSUE_WARNING) report->warning_count+=1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->issue_count>=UMI_PROJECT_WORKSPACE_VALIDATION_ISSUE_CAPACITY) return;
    issue=&report->issues[report->issue_count++];
    memset(issue,0,sizeof(*issue));
    issue->struct_size=(uint32_t)sizeof(*issue);
    issue->api_version=UMI_PROJECT_WORKSPACE_VALIDATION_API_VERSION;
    issue->severity=severity;
    copy_issue_text(issue->code,sizeof(issue->code),code);
    copy_issue_text(issue->project_id,sizeof(issue->project_id),project_id);
    copy_issue_text(issue->item_id,sizeof(issue->item_id),item_id);
    copy_issue_text(issue->message,sizeof(issue->message),message);
}

/* Provide the project exists operation used by this module and its client applications. */
static int project_exists(const UmiProjectWorkspace *workspace, const char *project_id)
{
    UmiProjectDescriptorSnapshot descriptor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_id==NULL || project_id[0]=='\0') return 0;
    return umi_project_descriptor_registry_find(
        workspace->descriptor,project_id,&descriptor)==UMI_STATUS_OK;
}

/*
 * Provide the validate configurations operation used by this module and its client
 * applications.
 */
static void validate_configurations(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i,j,active_count;
    UmiProjectConfigurationSnapshot item, other;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_configuration_registry_count(workspace->configuration);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_configuration_registry_at(workspace->configuration,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "configuration.orphan",item.project_id,item.id,
                      "Configuration refers to a project that does not exist.");
        }
        /* Apply this operation only while the related capability or state is available. */
        if (item.active==0) continue;
        active_count=0U;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j=0U;j<umi_project_configuration_registry_count(workspace->configuration);++j) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_project_configuration_registry_at(workspace->configuration,j,&other)==UMI_STATUS_OK &&
                same_project(other.project_id,item.project_id) && other.active!=0) ++active_count;
        }
        /* Apply this operation only while the related capability or state is available. */
        if (active_count>1U) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "configuration.multiple-active",item.project_id,item.id,
                      "Project contains more than one active configuration.");
            break;
        }
    }
}

/* Provide the validate targets operation used by this module and its client applications. */
static void validate_targets(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i,j,defaults;
    UmiProjectTargetSnapshot item,other;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_target_registry_count(workspace->target);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_target_registry_at(workspace->target,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "target.orphan",item.project_id,item.id,
                      "Target refers to a project that does not exist.");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.default_target==0) continue;
        defaults=0U;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j=0U;j<umi_project_target_registry_count(workspace->target);++j) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_project_target_registry_at(workspace->target,j,&other)==UMI_STATUS_OK &&
                same_project(other.project_id,item.project_id) &&
                other.default_target!=0 && other.enabled!=0) ++defaults;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (defaults>1U) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "target.multiple-defaults",item.project_id,item.id,
                      "Project contains more than one enabled default target.");
            break;
        }
    }
}

/* Provide the validate tasks operation used by this module and its client applications. */
static void validate_tasks(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i,j,defaults;
    UmiProjectTaskSnapshot item,other;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_task_registry_count(workspace->task);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_task_registry_at(workspace->task,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "task.orphan",item.project_id,item.id,
                      "Task refers to a project that does not exist.");
        }
        /* Apply this operation only while the related capability or state is available. */
        if (item.enabled!=0 && item.command[0]=='\0') {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "task.empty-command",item.project_id,item.id,
                      "Enabled task does not contain a command.");
        }
        /* Apply this operation only while the related capability or state is available. */
        if (item.default_task==0 || item.enabled==0) continue;
        defaults=0U;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j=0U;j<umi_project_task_registry_count(workspace->task);++j) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_project_task_registry_at(workspace->task,j,&other)==UMI_STATUS_OK &&
                same_project(other.project_id,item.project_id) &&
                other.default_task!=0 && other.enabled!=0) ++defaults;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (defaults>1U) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "task.multiple-defaults",item.project_id,item.id,
                      "Project contains more than one enabled default task.");
            break;
        }
    }
}

/*
 * Provide the validate launch profiles operation used by this module and its client
 * applications.
 */
static void validate_launch_profiles(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i,j,defaults;
    UmiProjectLaunchProfileSnapshot item,other;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_launch_profile_registry_count(workspace->launch_profile);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_launch_profile_registry_at(workspace->launch_profile,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "launch.orphan",item.project_id,item.id,
                      "Launch profile refers to a project that does not exist.");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.program[0]=='\0') {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "launch.empty-program",item.project_id,item.id,
                      "Launch profile does not contain a program.");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.default_profile==0) continue;
        defaults=0U;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j=0U;j<umi_project_launch_profile_registry_count(workspace->launch_profile);++j) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_project_launch_profile_registry_at(workspace->launch_profile,j,&other)==UMI_STATUS_OK &&
                same_project(other.project_id,item.project_id) &&
                other.default_profile!=0) ++defaults;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (defaults>1U) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "launch.multiple-defaults",item.project_id,item.id,
                      "Project contains more than one default launch profile.");
            break;
        }
    }
}

/*
 * Provide the validate environments operation used by this module and its client
 * applications.
 */
static void validate_environments(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i;
    UmiProjectEnvironmentSnapshot item;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_environment_registry_count(workspace->environment);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_environment_registry_at(workspace->environment,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "environment.orphan",item.project_id,item.id,
                      "Environment refers to a project that does not exist.");
        }
    }
}

/*
 * Provide the validate dependencies operation used by this module and its client
 * applications.
 */
static void validate_dependencies(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *report)
{
    size_t i;
    UmiProjectDependencySnapshot item;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_project_dependency_registry_count(workspace->dependency);++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_project_dependency_registry_at(workspace->dependency,i,&item)!=UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!project_exists(workspace,item.project_id)) {
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "dependency.orphan",item.project_id,item.id,
                      "Dependency refers to a project that does not exist.");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.optional==0 && item.resolved==0) {
            report->unresolved_required_dependency_count+=1U;
            add_issue(report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                      "dependency.unresolved",item.project_id,item.id,
                      "Required project dependency is unresolved.");
        }
    }
}

/* Check that project workspace satisfies its contract before another service relies on it. */
UmiStatus umi_project_workspace_validate(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *out_report)
{
    UmiProjectWorkspaceSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace==NULL || out_report==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report,0,sizeof(*out_report));
    out_report->struct_size=(uint32_t)sizeof(*out_report);
    out_report->api_version=UMI_PROJECT_WORKSPACE_VALIDATION_API_VERSION;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_snapshot(workspace,&snapshot)!=UMI_STATUS_OK)
        return UMI_STATUS_INTERNAL_ERROR;
    out_report->workspace_revision=snapshot.revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.descriptor_count==0U) {
        add_issue(out_report,UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                  "workspace.no-project","","",
                  "Workspace does not contain a project descriptor.");
    }
    validate_configurations(workspace,out_report);
    validate_targets(workspace,out_report);
    validate_tasks(workspace,out_report);
    validate_launch_profiles(workspace,out_report);
    validate_environments(workspace,out_report);
    validate_dependencies(workspace,out_report);
    out_report->valid=(out_report->error_count==0U);
    return UMI_STATUS_OK;
}
