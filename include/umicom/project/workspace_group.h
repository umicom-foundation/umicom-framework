/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_group.h
 * PURPOSE: Define project groups and deterministic reference ordering.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiProjectWorkspaceOrderSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_ids[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY][128];
    size_t project_count;
    size_t unresolved_reference_count;
    int has_cycle;
} UmiProjectWorkspaceOrderSnapshot;

UmiStatus umi_project_workspace_model_upsert_group(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceGroupSnapshot *group);
UmiStatus umi_project_workspace_model_find_group(
    const UmiProjectWorkspaceModel *model,
    const char *group_id,
    UmiProjectWorkspaceGroupSnapshot *out_group);
UmiStatus umi_project_workspace_model_group_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceGroupSnapshot *out_group);
size_t umi_project_workspace_model_group_count(
    const UmiProjectWorkspaceModel *model);
UmiStatus umi_project_workspace_model_resolve_build_order(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceOrderSnapshot *out_order);

#ifdef __cplusplus
}
#endif
#endif
