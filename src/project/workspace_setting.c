/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_setting.c
 * PURPOSE: Resolve workspace -> group -> project setting inheritance.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_setting.h"
#include "workspace_model_internal.h"

#include <stdint.h>
#include <string.h>

static size_t find_setting_index(const UmiProjectWorkspaceModel *model,
                                 const char *setting_id)
{
    size_t index;
    if (model == NULL || setting_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->setting_count; ++index)
        if (strcmp(model->settings[index].id, setting_id) == 0) return index;
    return SIZE_MAX;
}

UmiStatus umi_project_workspace_model_upsert_setting(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceSettingSnapshot *setting)
{
    size_t index;
    if (model == NULL || setting == NULL || setting->id[0] == '\0' ||
        setting->key[0] == '\0' ||
        setting->scope < UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE ||
        setting->scope > UMI_PROJECT_WORKSPACE_SETTING_PROJECT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (setting->scope != UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE &&
        setting->owner_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_setting_index(model, setting->id);
    if (index != SIZE_MAX &&
        strcmp(model->settings[index].owner_id, setting->owner_id) == 0 &&
        strcmp(model->settings[index].key, setting->key) == 0 &&
        strcmp(model->settings[index].value, setting->value) == 0 &&
        model->settings[index].scope == setting->scope &&
        model->settings[index].secret == setting->secret)
        return UMI_STATUS_OK;
    if (index == SIZE_MAX) {
        if (model->setting_count >= UMI_PROJECT_WORKSPACE_SETTING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = model->setting_count++;
    }
    model->settings[index] = *setting;
    model->settings[index].struct_size = (uint32_t)sizeof(model->settings[index]);
    model->settings[index].api_version = UMI_PROJECT_WORKSPACE_SETTING_API_VERSION;
    model->settings[index].id[127U] = '\0';
    model->settings[index].owner_id[127U] = '\0';
    model->settings[index].key[255U] = '\0';
    model->settings[index].value[1023U] = '\0';
    model->settings[index].revision = ++model->revision;
    return UMI_STATUS_OK;
}

size_t umi_project_workspace_model_setting_count(const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->setting_count : 0U;
}

UmiStatus umi_project_workspace_model_setting_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceSettingSnapshot *out_setting)
{
    if (model == NULL || out_setting == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->setting_count) return UMI_STATUS_NOT_FOUND;
    *out_setting = model->settings[index];
    return UMI_STATUS_OK;
}

static void write_resolved(
    const UmiProjectWorkspaceSettingSnapshot *setting,
    UmiProjectWorkspaceResolvedSetting *out_setting)
{
    memset(out_setting, 0, sizeof(*out_setting));
    out_setting->struct_size = (uint32_t)sizeof(*out_setting);
    out_setting->api_version = UMI_PROJECT_WORKSPACE_SETTING_API_VERSION;
    umi_project_workspace_model_copy_text(
        out_setting->key, sizeof(out_setting->key), setting->key);
    umi_project_workspace_model_copy_text(
        out_setting->value, sizeof(out_setting->value), setting->value);
    umi_project_workspace_model_copy_text(
        out_setting->source_id, sizeof(out_setting->source_id), setting->owner_id);
    out_setting->source_scope = setting->scope;
    out_setting->secret = setting->secret;
}

static const UmiProjectWorkspaceSettingSnapshot *find_scoped_setting(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceSettingScope scope,
    const char *owner_id,
    const char *key)
{
    size_t index;
    const UmiProjectWorkspaceSettingSnapshot *best = NULL;
    for (index = 0U; index < model->setting_count; ++index) {
        const UmiProjectWorkspaceSettingSnapshot *candidate = &model->settings[index];
        if (candidate->scope != scope || strcmp(candidate->key, key) != 0)
            continue;
        if (scope != UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE &&
            strcmp(candidate->owner_id, owner_id) != 0) continue;
        if (best == NULL || candidate->revision > best->revision) best = candidate;
    }
    return best;
}

UmiStatus umi_project_workspace_model_resolve_setting(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *key,
    UmiProjectWorkspaceResolvedSetting *out_setting)
{
    UmiProjectWorkspaceMemberSnapshot member;
    const UmiProjectWorkspaceSettingSnapshot *setting;
    if (model == NULL || project_id == NULL || key == NULL || out_setting == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_project_workspace_model_find_project_member(
            model, project_id, &member) != UMI_STATUS_OK)
        return UMI_STATUS_NOT_FOUND;
    setting = find_scoped_setting(model, UMI_PROJECT_WORKSPACE_SETTING_PROJECT,
                                  project_id, key);
    if (setting == NULL && member.group_id[0] != '\0')
        setting = find_scoped_setting(model, UMI_PROJECT_WORKSPACE_SETTING_GROUP,
                                      member.group_id, key);
    if (setting == NULL)
        setting = find_scoped_setting(model, UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE,
                                      "", key);
    if (setting == NULL) return UMI_STATUS_NOT_FOUND;
    write_resolved(setting, out_setting);
    return UMI_STATUS_OK;
}

UmiStatus umi_project_workspace_model_resolve_variable(
    const UmiProjectWorkspaceModel *model,
    const char *project_id,
    const char *name,
    UmiProjectWorkspaceResolvedSetting *out_setting)
{
    size_t index;
    UmiProjectVariableSnapshot candidate;
    UmiProjectVariableSnapshot best;
    int have_best = 0;
    if (model == NULL || project_id == NULL || name == NULL || out_setting == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U;
         index < umi_project_variable_registry_count(
             umi_project_workspace_variable(model->projects));
         ++index) {
        if (umi_project_variable_registry_at(
                umi_project_workspace_variable(model->projects),
                index, &candidate) != UMI_STATUS_OK ||
            strcmp(candidate.name, name) != 0) continue;
        if (candidate.project_id[0] == '\0' ||
            strcmp(candidate.project_id, project_id) == 0) {
            int candidate_is_project = candidate.project_id[0] != '\0';
            int best_is_project = have_best && best.project_id[0] != '\0';
            if (!have_best ||
                (candidate_is_project && !best_is_project) ||
                (candidate_is_project == best_is_project &&
                 candidate.revision > best.revision)) {
                best = candidate;
                have_best = 1;
            }
        }
    }
    if (!have_best) return UMI_STATUS_NOT_FOUND;
    memset(out_setting, 0, sizeof(*out_setting));
    out_setting->struct_size = (uint32_t)sizeof(*out_setting);
    out_setting->api_version = UMI_PROJECT_WORKSPACE_SETTING_API_VERSION;
    umi_project_workspace_model_copy_text(
        out_setting->key, sizeof(out_setting->key), best.name);
    umi_project_workspace_model_copy_text(
        out_setting->value, sizeof(out_setting->value), best.value);
    umi_project_workspace_model_copy_text(
        out_setting->source_id, sizeof(out_setting->source_id), best.project_id);
    out_setting->source_scope = best.project_id[0] != '\0'
        ? UMI_PROJECT_WORKSPACE_SETTING_PROJECT
        : UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE;
    out_setting->secret = best.secret;
    return UMI_STATUS_OK;
}
