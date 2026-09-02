/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_model.h
 *
 * PURPOSE:
 *   Define the multi-root workspace catalogue and deterministic
 *   active-project contract used by Studio and future Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_MODEL_H
#define UMICOM_PROJECT_WORKSPACE_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/project/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_MODEL_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_ROOT_CAPACITY 64U
#define UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY 256U

/**
 * Represent the project workspace model data shared with callers of this public contract.
 */
typedef struct UmiProjectWorkspaceModel UmiProjectWorkspaceModel;

/**
 * Represent the project workspace root snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceRootSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char path[2048];
    char label[256];
    int32_t order;
    int enabled;
    int read_only;
    uint64_t revision;
} UmiProjectWorkspaceRootSnapshot;

/**
 * Represent the project workspace member snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceMemberSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char root_id[128];
    char project_id[128];
    char group_id[128];
    int32_t order;
    int enabled;
    uint64_t revision;
} UmiProjectWorkspaceMemberSnapshot;

/**
 * Represent the project workspace model snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t root_count;
    size_t member_count;
    size_t enabled_root_count;
    size_t enabled_project_count;
    char active_project_id[128];
    int has_active_project;
} UmiProjectWorkspaceModelSnapshot;

/**
 * Initialise project workspace model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_workspace_model_create(
    UmiProjectWorkspace *project_workspace,
    UmiProjectWorkspaceModel **out_model);
/**
 * Release or reset state held by project workspace model so the same storage can be reused
 * safely.
 */
void umi_project_workspace_model_destroy(UmiProjectWorkspaceModel *model);
/**
 * Provide the project workspace model projects operation used by this module and its
 * client applications.
 */
UmiProjectWorkspace *umi_project_workspace_model_projects(
    const UmiProjectWorkspaceModel *model);
/**
 * Provide the project workspace model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_snapshot(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceModelSnapshot *out_snapshot);

/**
 * Provide the project workspace model upsert root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_root(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceRootSnapshot *root);
/**
 * Provide the project workspace model remove root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_remove_root(
    UmiProjectWorkspaceModel *model,
    const char *root_id);
/**
 * Provide the project workspace model find root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_find_root(
    const UmiProjectWorkspaceModel *model,
    const char *root_id,
    UmiProjectWorkspaceRootSnapshot *out_root);
/**
 * Find project workspace model root while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_workspace_model_root_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceRootSnapshot *out_root);
/**
 * Return the number of records represented by project workspace model root without
 * changing their state.
 */
size_t umi_project_workspace_model_root_count(
    const UmiProjectWorkspaceModel *model);

/**
 * Provide the project workspace model upsert member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_member(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceMemberSnapshot *member);
/**
 * Provide the project workspace model remove member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_remove_member(
    UmiProjectWorkspaceModel *model,
    const char *member_id);
/**
 * Provide the project workspace model find member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_find_member(
    const UmiProjectWorkspaceModel *model,
    const char *member_id,
    UmiProjectWorkspaceMemberSnapshot *out_member);
/**
 * Provide the project workspace model find project member operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_model_find_project_member(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    UmiProjectWorkspaceMemberSnapshot *out_member);
/**
 * Find project workspace model member while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_workspace_model_member_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceMemberSnapshot *out_member);
/**
 * Return the number of records represented by project workspace model member without
 * changing their state.
 */
size_t umi_project_workspace_model_member_count(
    const UmiProjectWorkspaceModel *model);

/**
 * Provide the project workspace model set active project operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_set_active_project(
    UmiProjectWorkspaceModel *model,
    const char *project_id);
/**
 * Provide the project workspace model resolve active project operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_model_resolve_active_project(
    const UmiProjectWorkspaceModel *model,
    UmiProjectDescriptorSnapshot *out_project);

#ifdef __cplusplus
}
#endif
#endif
