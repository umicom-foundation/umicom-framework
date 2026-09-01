/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace.h
 *
 * PURPOSE:
 *   Define the reusable project workspace that owns project metadata, targets, dependencies, tasks, launch profiles and build graph records.
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
#ifndef FRAMEWORK_INCLUDE_UMICOM_PROJECT_WORKSPACE_H
#define FRAMEWORK_INCLUDE_UMICOM_PROJECT_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/project/descriptor.h"
#include "umicom/project/target.h"
#include "umicom/project/configuration.h"
#include "umicom/project/dependency.h"
#include "umicom/project/file_set.h"
#include "umicom/project/task.h"
#include "umicom/project/launch_profile.h"
#include "umicom/project/environment.h"
#include "umicom/project/reference.h"
#include "umicom/project/template.h"
#include "umicom/project/variable.h"
#include "umicom/project/capability.h"
#include "umicom/project/build_node.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProjectWorkspace UmiProjectWorkspace;
typedef struct UmiProjectWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t item_count;
    size_t descriptor_count;
    size_t target_count;
    size_t configuration_count;
    size_t dependency_count;
    size_t file_set_count;
    size_t task_count;
    size_t launch_profile_count;
    size_t environment_count;
    size_t reference_count;
    size_t template_count;
    size_t variable_count;
    size_t capability_count;
    size_t build_node_count;
} UmiProjectWorkspaceSnapshot;

UmiStatus umi_project_workspace_create(UmiProjectWorkspace **out_owner);
void umi_project_workspace_destroy(UmiProjectWorkspace *owner);
UmiStatus umi_project_workspace_snapshot(const UmiProjectWorkspace *owner, UmiProjectWorkspaceSnapshot *out_snapshot);
UmiProjectDescriptorRegistry *umi_project_workspace_descriptor(UmiProjectWorkspace *owner);
UmiProjectTargetRegistry *umi_project_workspace_target(UmiProjectWorkspace *owner);
UmiProjectConfigurationRegistry *umi_project_workspace_configuration(UmiProjectWorkspace *owner);
UmiProjectDependencyRegistry *umi_project_workspace_dependency(UmiProjectWorkspace *owner);
UmiProjectFileSetRegistry *umi_project_workspace_file_set(UmiProjectWorkspace *owner);
UmiProjectTaskRegistry *umi_project_workspace_task(UmiProjectWorkspace *owner);
UmiProjectLaunchProfileRegistry *umi_project_workspace_launch_profile(UmiProjectWorkspace *owner);
UmiProjectEnvironmentRegistry *umi_project_workspace_environment(UmiProjectWorkspace *owner);
UmiProjectReferenceRegistry *umi_project_workspace_reference(UmiProjectWorkspace *owner);
UmiProjectTemplateRegistry *umi_project_workspace_template(UmiProjectWorkspace *owner);
UmiProjectVariableRegistry *umi_project_workspace_variable(UmiProjectWorkspace *owner);
UmiProjectCapabilityRegistry *umi_project_workspace_capability(UmiProjectWorkspace *owner);
UmiProjectBuildNodeRegistry *umi_project_workspace_build_node(UmiProjectWorkspace *owner);

#ifdef __cplusplus
}
#endif
#endif
