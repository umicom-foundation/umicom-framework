/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_setting.h
 * PURPOSE: Define inherited workspace, group and project setting values.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_SETTING_H
#define UMICOM_PROJECT_WORKSPACE_SETTING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/project/workspace_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_SETTING_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_SETTING_CAPACITY 512U

/**
 * List the named project workspace setting scope values accepted by this public contract.
 */
typedef enum UmiProjectWorkspaceSettingScope {
    UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE = 1,
    UMI_PROJECT_WORKSPACE_SETTING_GROUP = 2,
    UMI_PROJECT_WORKSPACE_SETTING_PROJECT = 3
} UmiProjectWorkspaceSettingScope;

/**
 * Represent the project workspace setting snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceSettingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char owner_id[128];
    char key[256];
    char value[1024];
    UmiProjectWorkspaceSettingScope scope;
    int secret;
    uint64_t revision;
} UmiProjectWorkspaceSettingSnapshot;

/**
 * Represent the project workspace resolved setting data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceResolvedSetting {
    uint32_t struct_size;
    uint32_t api_version;
    char key[256];
    char value[1024];
    char source_id[128];
    UmiProjectWorkspaceSettingScope source_scope;
    int secret;
} UmiProjectWorkspaceResolvedSetting;

/**
 * Provide the project workspace model upsert setting operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_setting(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceSettingSnapshot *setting);
/**
 * Return the number of records represented by project workspace model setting without
 * changing their state.
 */
size_t umi_project_workspace_model_setting_count(
    const UmiProjectWorkspaceModel *model);
/**
 * Find project workspace model setting while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_workspace_model_setting_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceSettingSnapshot *out_setting);
/**
 * Provide the project workspace model resolve setting operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_resolve_setting(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *key,
    UmiProjectWorkspaceResolvedSetting *out_setting);
/**
 * Provide the project workspace model resolve variable operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_resolve_variable(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *name,
    UmiProjectWorkspaceResolvedSetting *out_setting);

#ifdef __cplusplus
}
#endif
#endif
