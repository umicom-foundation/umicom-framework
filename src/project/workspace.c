/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace.c
 *
 * PURPOSE:
 *   Implement the reusable project workspace that owns project metadata, targets, dependencies, tasks, launch profiles and build graph records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/project/workspace.h"

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

UmiStatus umi_project_workspace_create(UmiProjectWorkspace **out_owner)
{
    UmiProjectWorkspace *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiProjectWorkspace *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_project_descriptor_registry_create(&owner->descriptor);
    if (status == UMI_STATUS_OK) status = umi_project_target_registry_create(&owner->target);
    if (status == UMI_STATUS_OK) status = umi_project_configuration_registry_create(&owner->configuration);
    if (status == UMI_STATUS_OK) status = umi_project_dependency_registry_create(&owner->dependency);
    if (status == UMI_STATUS_OK) status = umi_project_file_set_registry_create(&owner->file_set);
    if (status == UMI_STATUS_OK) status = umi_project_task_registry_create(&owner->task);
    if (status == UMI_STATUS_OK) status = umi_project_launch_profile_registry_create(&owner->launch_profile);
    if (status == UMI_STATUS_OK) status = umi_project_environment_registry_create(&owner->environment);
    if (status == UMI_STATUS_OK) status = umi_project_reference_registry_create(&owner->reference);
    if (status == UMI_STATUS_OK) status = umi_project_template_registry_create(&owner->template);
    if (status == UMI_STATUS_OK) status = umi_project_variable_registry_create(&owner->variable);
    if (status == UMI_STATUS_OK) status = umi_project_capability_registry_create(&owner->capability);
    if (status == UMI_STATUS_OK) status = umi_project_build_node_registry_create(&owner->build_node);
    if (status != UMI_STATUS_OK) { umi_project_workspace_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

void umi_project_workspace_destroy(UmiProjectWorkspace *owner)
{
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

UmiStatus umi_project_workspace_snapshot(const UmiProjectWorkspace *owner, UmiProjectWorkspaceSnapshot *out_snapshot)
{
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=owner->revision;
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

UmiProjectDescriptorRegistry *umi_project_workspace_descriptor(UmiProjectWorkspace *owner) { return owner != NULL ? owner->descriptor : NULL; }
UmiProjectTargetRegistry *umi_project_workspace_target(UmiProjectWorkspace *owner) { return owner != NULL ? owner->target : NULL; }
UmiProjectConfigurationRegistry *umi_project_workspace_configuration(UmiProjectWorkspace *owner) { return owner != NULL ? owner->configuration : NULL; }
UmiProjectDependencyRegistry *umi_project_workspace_dependency(UmiProjectWorkspace *owner) { return owner != NULL ? owner->dependency : NULL; }
UmiProjectFileSetRegistry *umi_project_workspace_file_set(UmiProjectWorkspace *owner) { return owner != NULL ? owner->file_set : NULL; }
UmiProjectTaskRegistry *umi_project_workspace_task(UmiProjectWorkspace *owner) { return owner != NULL ? owner->task : NULL; }
UmiProjectLaunchProfileRegistry *umi_project_workspace_launch_profile(UmiProjectWorkspace *owner) { return owner != NULL ? owner->launch_profile : NULL; }
UmiProjectEnvironmentRegistry *umi_project_workspace_environment(UmiProjectWorkspace *owner) { return owner != NULL ? owner->environment : NULL; }
UmiProjectReferenceRegistry *umi_project_workspace_reference(UmiProjectWorkspace *owner) { return owner != NULL ? owner->reference : NULL; }
UmiProjectTemplateRegistry *umi_project_workspace_template(UmiProjectWorkspace *owner) { return owner != NULL ? owner->template : NULL; }
UmiProjectVariableRegistry *umi_project_workspace_variable(UmiProjectWorkspace *owner) { return owner != NULL ? owner->variable : NULL; }
UmiProjectCapabilityRegistry *umi_project_workspace_capability(UmiProjectWorkspace *owner) { return owner != NULL ? owner->capability : NULL; }
UmiProjectBuildNodeRegistry *umi_project_workspace_build_node(UmiProjectWorkspace *owner) { return owner != NULL ? owner->build_node : NULL; }
