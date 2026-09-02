/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_group.h
 * PURPOSE: Define project groups and deterministic reference ordering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_GROUP_H
#define UMICOM_PROJECT_WORKSPACE_GROUP_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/project/workspace_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_GROUP_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_GROUP_CAPACITY 64U

/**
 * Represent the project workspace group snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceGroupSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char parent_group_id[128];
    int32_t order;
    int enabled;
    uint64_t revision;
} UmiProjectWorkspaceGroupSnapshot;

/**
 * Represent the project workspace order snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceOrderSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_ids[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY][128];
    size_t project_count;
    size_t unresolved_reference_count;
    int has_cycle;
} UmiProjectWorkspaceOrderSnapshot;

/**
 * Provide the project workspace model upsert group operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_group(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceGroupSnapshot *group);
/**
 * Provide the project workspace model find group operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_find_group(
    const UmiProjectWorkspaceModel *model,
    const char *group_id,
    UmiProjectWorkspaceGroupSnapshot *out_group);
/**
 * Find project workspace model group while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_workspace_model_group_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceGroupSnapshot *out_group);
/**
 * Return the number of records represented by project workspace model group without
 * changing their state.
 */
size_t umi_project_workspace_model_group_count(
    const UmiProjectWorkspaceModel *model);
/**
 * Provide the project workspace model resolve build order operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_model_resolve_build_order(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceOrderSnapshot *out_order);

#ifdef __cplusplus
}
#endif
#endif
