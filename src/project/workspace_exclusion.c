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

/*
 * Provide the find exclusion index operation used by this module and its client
 * applications.
 */
static size_t find_exclusion_index(const UmiProjectWorkspaceModel *model,
                                   const char *exclusion_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || exclusion_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->exclusion_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->exclusions[index].id, exclusion_id) == 0) return index;
    return SIZE_MAX;
}

/*
 * Provide the project workspace model upsert exclusion operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_upsert_exclusion(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceExclusionSnapshot *exclusion)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || exclusion == NULL || exclusion->id[0] == '\0' ||
        exclusion->pattern[0] == '\0' ||
        exclusion->kind < UMI_PROJECT_WORKSPACE_EXCLUDE_EXACT_NAME ||
        exclusion->kind > UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_exclusion_index(model, exclusion->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX &&
        strcmp(model->exclusions[index].pattern, exclusion->pattern) == 0 &&
        model->exclusions[index].kind == exclusion->kind &&
        model->exclusions[index].enabled == exclusion->enabled)
        return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Return the number of records represented by project workspace model exclusion without
 * changing their state.
 */
size_t umi_project_workspace_model_exclusion_count(
    const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->exclusion_count : 0U;
}

/*
 * Find project workspace model exclusion while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_workspace_model_exclusion_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceExclusionSnapshot *out_exclusion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_exclusion == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->exclusion_count) return UMI_STATUS_NOT_FOUND;
    *out_exclusion = model->exclusions[index];
    return UMI_STATUS_OK;
}

/* Provide the path name operation used by this module and its client applications. */
static const char *path_name(const char *path)
{
    const char *slash;
    const char *backslash;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return "";
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slash == NULL) slash = path - 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backslash == NULL) backslash = path - 1;
    return (slash > backslash ? slash : backslash) + 1;
}

/* Provide the ends with operation used by this module and its client applications. */
static int ends_with(const char *text, const char *suffix)
{
    size_t text_length = strlen(text);
    size_t suffix_length = strlen(suffix);
    return suffix_length <= text_length &&
           strcmp(text + text_length - suffix_length, suffix) == 0;
}

/* Provide the has segment operation used by this module and its client applications. */
static int has_segment(const char *path, const char *segment)
{
    const char *cursor = path;
    size_t length = strlen(segment);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((cursor = strstr(cursor, segment)) != NULL) {
        int left = cursor == path || cursor[-1] == '/' || cursor[-1] == '\\';
        int right = cursor[length] == '\0' || cursor[length] == '/' ||
                    cursor[length] == '\\';
        /* Apply this branch only when its contract condition is satisfied. */
        if (left && right) return 1;
        cursor += length > 0U ? length : 1U;
    }
    return 0;
}

/*
 * Provide the project workspace model path excluded operation used by this module and its
 * client applications.
 */
int umi_project_workspace_model_path_excluded(
    const UmiProjectWorkspaceModel *model,
    const char *relative_path,
    int is_directory)
{
    size_t index;
    const char *name;
    (void)is_directory;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || relative_path == NULL) return 0;
    name = path_name(relative_path);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->exclusion_count; ++index) {
        const UmiProjectWorkspaceExclusionSnapshot *rule = &model->exclusions[index];
        /* Apply this operation only while the related capability or state is available. */
        if (rule->enabled == 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_EXACT_NAME &&
            strcmp(name, rule->pattern) == 0) return 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_PREFIX &&
            strncmp(name, rule->pattern, strlen(rule->pattern)) == 0) return 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_SUFFIX &&
            ends_with(name, rule->pattern)) return 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->kind == UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT &&
            has_segment(relative_path, rule->pattern)) return 1;
    }
    return 0;
}
