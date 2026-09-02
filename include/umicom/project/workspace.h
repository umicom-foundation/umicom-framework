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

/**
 * Represent the project workspace data shared with callers of this public contract.
 */
typedef struct UmiProjectWorkspace UmiProjectWorkspace;
/**
 * Represent the project workspace snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise project workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_project_workspace_create(UmiProjectWorkspace **out_owner);
/**
 * Release or reset state held by project workspace so the same storage can be reused
 * safely.
 */
void umi_project_workspace_destroy(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_workspace_snapshot(const UmiProjectWorkspace *owner, UmiProjectWorkspaceSnapshot *out_snapshot);
/**
 * Provide the project workspace descriptor operation used by this module and its client
 * applications.
 */
UmiProjectDescriptorRegistry *umi_project_workspace_descriptor(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace target operation used by this module and its client
 * applications.
 */
UmiProjectTargetRegistry *umi_project_workspace_target(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace configuration operation used by this module and its client
 * applications.
 */
UmiProjectConfigurationRegistry *umi_project_workspace_configuration(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace dependency operation used by this module and its client
 * applications.
 */
UmiProjectDependencyRegistry *umi_project_workspace_dependency(UmiProjectWorkspace *owner);
/**
 * Copy project workspace file into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiProjectFileSetRegistry *umi_project_workspace_file_set(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace task operation used by this module and its client
 * applications.
 */
UmiProjectTaskRegistry *umi_project_workspace_task(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace launch profile operation used by this module and its
 * client applications.
 */
UmiProjectLaunchProfileRegistry *umi_project_workspace_launch_profile(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace environment operation used by this module and its client
 * applications.
 */
UmiProjectEnvironmentRegistry *umi_project_workspace_environment(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace reference operation used by this module and its client
 * applications.
 */
UmiProjectReferenceRegistry *umi_project_workspace_reference(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace template operation used by this module and its client
 * applications.
 */
UmiProjectTemplateRegistry *umi_project_workspace_template(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace variable operation used by this module and its client
 * applications.
 */
UmiProjectVariableRegistry *umi_project_workspace_variable(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace capability operation used by this module and its client
 * applications.
 */
UmiProjectCapabilityRegistry *umi_project_workspace_capability(UmiProjectWorkspace *owner);
/**
 * Provide the project workspace build node operation used by this module and its client
 * applications.
 */
UmiProjectBuildNodeRegistry *umi_project_workspace_build_node(UmiProjectWorkspace *owner);

#ifdef __cplusplus
}
#endif
#endif
