/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_exclusion.c
 * PURPOSE: Implement bounded, portable exclusion matching.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_exclusion.h"
#include "workspace_model_internal.h"

#include <stdint.h>
#include <string.h>

static size_t find_exclusion_index(const UmiProjectWorkspaceModel *model,
                                   const char *exclusion_id)
{
    size_t index;
    if (model == NULL || exclusion_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->exclusion_count; ++index)
        if (strcmp(model->exclusions[index].id, exclusion_id) == 0) return index;
    return SIZE_MAX;
}

UmiStatus umi_project_workspace_model_upsert_exclusion(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceExclusionSnapshot *exclusion)
{
    size_t index;
    if (model == NULL || exclusion == NULL || exclusion->id[0] == '\0' ||
        exclusion->pattern[0] == '\0' ||
        exclusion->kind < UMI_PROJECT_WORKSPACE_EXCLUDE_EXACT_NAME ||
        exclusion->kind > UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_exclusion_index(model, exclusion->id);
    if (index != SIZE_MAX &&
        strcmp(model->exclusions[index].pattern, exclusion->pattern) == 0 &&
        model->exclusions[index].kind == exclusion->kind &&
        model->exclusions[index].enabled == exclusion->enabled)
        return UMI_STATUS_OK;
    if (index == SIZE_MAX) {
        if (model->exclusion_count >= UMI_PROJECT_WORKSPACE_EXCLUSION_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = model->exclusion_count++;
    }
    model->exclusions[index] = *exclusion;
    model->exclusions[index].struct_size =
        (uint32_t)sizeof(model->exclusions[index]);
    model->exclusions[index].api_version =
        UMI_PROJECT_WORKSPACE_EXCLUSION_API_VERSION;
    model->exclusions[index].id[127U] = '\0';
    model->exclusions[index].pattern[511U] = '\0';
    model->exclusions[index].revision = ++model->revision;
    return UMI_STATUS_OK;
}

size_t umi_project_workspace_model_exclusion_count(
    const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->exclusion_count : 0U;
}

UmiStatus umi_project_workspace_model_exclusion_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceExclusionSnapshot *out_exclusion)
{
    if (model == NULL || out_exclusion == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->exclusion_count) return UMI_STATUS_NOT_FOUND;
    *out_exclusion = model->exclusions[index];
    return UMI_STATUS_OK;
}

static const char *path_name(const char *path)
{
    const char *slash;
    const char *backslash;
    if (path == NULL) return "";
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    if (slash == NULL) slash = path - 1;
    if (backslash == NULL) backslash = path - 1;
    return (slash > backslash ? slash : backslash) + 1;
}

static int ends_with(const char *text, const char *suffix)
{
    size_t text_length = strlen(text);
    size_t suffix_length = strlen(suffix);
    return suffix_length <= text_length &&
           strcmp(text + text_length - suffix_length, suffix) == 0;
}

static int has_segment(const char *path, const char *segment)
{
    const char *cursor = path;
    size_t length = strlen(segment);
    while ((cursor = strstr(cursor, segment)) != NULL) {
        int left = cursor == path || cursor[-1] == '/' || cursor[-1] == '\\';
        int right = cursor[length] == '\0' || cursor[length] == '/' ||
                    cursor[length] == '\\';
        if (left && right) return 1;
        cursor += length > 0U ? length : 1U;
    }
    return 0;
}

int umi_project_workspace_model_path_excluded(
    const UmiProjectWorkspaceModel *model,
    const char *relative_path,
    int is_directory)
{
    size_t index;
    const char *name;
    (void)is_directory;
    if (model == NULL || relative_path == NULL) return 0;
    name = path_name(relative_path);
    for (index = 0U; index < model->exclusion_count; ++index) {
        const UmiProjectWorkspaceExclusionSnapshot *rule = &model->exclusions[index];
        if (rule->enabled == 0) continue;
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_EXACT_NAME &&
            strcmp(name, rule->pattern) == 0) return 1;
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_PREFIX &&
            strncmp(name, rule->pattern, strlen(rule->pattern)) == 0) return 1;
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_SUFFIX &&
            ends_with(name, rule->pattern)) return 1;
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT &&
            has_segment(relative_path, rule->pattern)) return 1;
    }
    return 0;
}
