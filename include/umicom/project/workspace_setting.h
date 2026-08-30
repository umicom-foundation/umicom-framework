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

typedef enum UmiProjectWorkspaceSettingScope {
    UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE = 1,
    UMI_PROJECT_WORKSPACE_SETTING_GROUP = 2,
    UMI_PROJECT_WORKSPACE_SETTING_PROJECT = 3
} UmiProjectWorkspaceSettingScope;

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

typedef struct UmiProjectWorkspaceResolvedSetting {
    uint32_t struct_size;
    uint32_t api_version;
    char key[256];
    char value[1024];
    char source_id[128];
    UmiProjectWorkspaceSettingScope source_scope;
    int secret;
} UmiProjectWorkspaceResolvedSetting;

UmiStatus umi_project_workspace_model_upsert_setting(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceSettingSnapshot *setting);
size_t umi_project_workspace_model_setting_count(
    const UmiProjectWorkspaceModel *model);
UmiStatus umi_project_workspace_model_setting_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceSettingSnapshot *out_setting);
UmiStatus umi_project_workspace_model_resolve_setting(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *key,
    UmiProjectWorkspaceResolvedSetting *out_setting);
UmiStatus umi_project_workspace_model_resolve_variable(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *name,
    UmiProjectWorkspaceResolvedSetting *out_setting);

#ifdef __cplusplus
}
#endif
#endif
