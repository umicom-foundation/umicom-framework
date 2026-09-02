/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_model.c
 * PURPOSE: Implement multi-root catalogue and deterministic project selection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_model.h"
#include "workspace_model_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the project workspace model copy text operation used by this module and its
 * client applications.
 */
void umi_project_workspace_model_copy_text(
    char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the project workspace model find root index operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_model_find_root_index(
    const UmiProjectWorkspaceModel *model, const char *root_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || root_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->root_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->roots[index].id, root_id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Provide the project workspace model find member index operation used by this module and
 * its client applications.
 */
size_t umi_project_workspace_model_find_member_index(
    const UmiProjectWorkspaceModel *model, const char *member_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || member_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->members[index].id, member_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the root enabled operation used by this module and its client applications. */
static int root_enabled(const UmiProjectWorkspaceModel *model, const char *root_id)
{
    size_t index = umi_project_workspace_model_find_root_index(model, root_id);
    return index != SIZE_MAX && model->roots[index].enabled != 0;
}

/* Provide the group enabled operation used by this module and its client applications. */
static int group_enabled(const UmiProjectWorkspaceModel *model,
                         const char *group_id)
{
    size_t visited = 0U;
    const char *current = group_id;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (current != NULL && current[0] != '\0') {
        size_t index = umi_project_workspace_model_find_group_index(
            model, current);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == SIZE_MAX) return 1; /* Forward-declared group identity. */
        if (model->groups[index].enabled == 0) return 0;
        current = model->groups[index].parent_group_id;
        visited += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (visited > model->group_count) return 0; /* Parent cycle. */
    }
    return 1;
}

/* Provide the same text operation used by this module and its client applications. */
static int same_text(const char *left, const char *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL) left = "";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) right = "";
    return strcmp(left, right) == 0;
}

/*
 * Provide the project workspace model member is selectable operation used by this module
 * and its client applications.
 */
int umi_project_workspace_model_member_is_selectable(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceMemberSnapshot *member)
{
    UmiProjectDescriptorSnapshot project;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || member == NULL || member->enabled == 0 ||
        !root_enabled(model, member->root_id) ||
        !group_enabled(model, member->group_id)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_descriptor_registry_find(
            umi_project_workspace_descriptor(model->projects),
            member->project_id, &project) != UMI_STATUS_OK) return 0;
    return project.enabled != 0;
}

/*
 * Initialise project workspace model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_workspace_model_create(
    UmiProjectWorkspace *project_workspace,
    UmiProjectWorkspaceModel **out_model)
{
    UmiProjectWorkspaceModel *model;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_workspace == NULL || out_model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiProjectWorkspaceModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->projects = project_workspace;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by project workspace model so the same storage can be reused
 * safely.
 */
void umi_project_workspace_model_destroy(UmiProjectWorkspaceModel *model)
{
    free(model);
}

/*
 * Provide the project workspace model projects operation used by this module and its
 * client applications.
 */
UmiProjectWorkspace *umi_project_workspace_model_projects(
    const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->projects : NULL;
}

/*
 * Provide the project workspace model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_snapshot(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceModelSnapshot *out_snapshot)
{
    size_t index;
    UmiProjectDescriptorSnapshot active;
    UmiProjectWorkspaceSnapshot projects;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    out_snapshot->revision = model->revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_snapshot(model->projects, &projects) ==
        UMI_STATUS_OK)
        out_snapshot->revision += projects.revision;
    out_snapshot->root_count = model->root_count;
    out_snapshot->member_count = model->member_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->root_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->roots[index].enabled != 0) out_snapshot->enabled_root_count += 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->member_count; ++index)
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_project_workspace_model_member_is_selectable(
                model, &model->members[index]))
            out_snapshot->enabled_project_count += 1U;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_project_workspace_model_resolve_active_project(model, &active) ==
        UMI_STATUS_OK) {
        umi_project_workspace_model_copy_text(
            out_snapshot->active_project_id,
            sizeof(out_snapshot->active_project_id), active.id);
        out_snapshot->has_active_project = 1;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model upsert root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_root(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceRootSnapshot *root)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || root == NULL || root->id[0] == '\0' ||
        root->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_root_index(model, root->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX &&
        same_text(model->roots[index].path, root->path) &&
        same_text(model->roots[index].label, root->label) &&
        model->roots[index].order == root->order &&
        model->roots[index].enabled == root->enabled &&
        model->roots[index].read_only == root->read_only)
        return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->root_count >= UMI_PROJECT_WORKSPACE_ROOT_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = model->root_count++;
    }
    model->roots[index] = *root;
    model->roots[index].struct_size = (uint32_t)sizeof(model->roots[index]);
    model->roots[index].api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    model->roots[index].id[127U] = '\0';
    model->roots[index].path[2047U] = '\0';
    model->roots[index].label[255U] = '\0';
    model->roots[index].revision = ++model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model remove root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_remove_root(
    UmiProjectWorkspaceModel *model, const char *root_id)
{
    size_t index;
    size_t member_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || root_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_root_index(model, root_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (member_index = 0U; member_index < model->member_count; ++member_index)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(model->members[member_index].root_id, root_id) == 0)
            return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->root_count)
        memmove(&model->roots[index], &model->roots[index + 1U],
                (model->root_count - index - 1U) * sizeof(model->roots[0]));
    model->root_count -= 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model find root operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_find_root(
    const UmiProjectWorkspaceModel *model, const char *root_id,
    UmiProjectWorkspaceRootSnapshot *out_root)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || root_id == NULL || out_root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_root_index(model, root_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_root = model->roots[index];
    return UMI_STATUS_OK;
}

/*
 * Find project workspace model root while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_workspace_model_root_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceRootSnapshot *out_root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->root_count) return UMI_STATUS_NOT_FOUND;
    *out_root = model->roots[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by project workspace model root without
 * changing their state.
 */
size_t umi_project_workspace_model_root_count(const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->root_count : 0U;
}

/*
 * Provide the project workspace model upsert member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_upsert_member(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceMemberSnapshot *member)
{
    UmiProjectDescriptorSnapshot project;
    size_t index;
    size_t member_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || member == NULL || member->id[0] == '\0' ||
        member->root_id[0] == '\0' || member->project_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_project_workspace_model_find_root_index(model, member->root_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_descriptor_registry_find(
            umi_project_workspace_descriptor(model->projects),
            member->project_id, &project) != UMI_STATUS_OK)
        return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (member_index = 0U; member_index < model->member_count; ++member_index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(model->members[member_index].project_id,
                   member->project_id) == 0 &&
            strcmp(model->members[member_index].id, member->id) != 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    index = umi_project_workspace_model_find_member_index(model, member->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX &&
        same_text(model->members[index].root_id, member->root_id) &&
        same_text(model->members[index].project_id, member->project_id) &&
        same_text(model->members[index].group_id, member->group_id) &&
        model->members[index].order == member->order &&
        model->members[index].enabled == member->enabled)
        return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->member_count >= UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = model->member_count++;
    }
    model->members[index] = *member;
    model->members[index].struct_size = (uint32_t)sizeof(model->members[index]);
    model->members[index].api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    model->members[index].id[127U] = '\0';
    model->members[index].root_id[127U] = '\0';
    model->members[index].project_id[127U] = '\0';
    model->members[index].group_id[127U] = '\0';
    model->members[index].revision = ++model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model remove member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_remove_member(
    UmiProjectWorkspaceModel *model, const char *member_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || member_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_member_index(model, member_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(model->active_project_id, model->members[index].project_id) == 0)
        model->active_project_id[0] = '\0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->member_count)
        memmove(&model->members[index], &model->members[index + 1U],
                (model->member_count - index - 1U) * sizeof(model->members[0]));
    model->member_count -= 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model find member operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_find_member(
    const UmiProjectWorkspaceModel *model, const char *member_id,
    UmiProjectWorkspaceMemberSnapshot *out_member)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || member_id == NULL || out_member == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_member_index(model, member_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_member = model->members[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace model find project member operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_model_find_project_member(
    const UmiProjectWorkspaceModel *model, const char *project_id,
    UmiProjectWorkspaceMemberSnapshot *out_member)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || out_member == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->members[index].project_id, project_id) == 0) {
            *out_member = model->members[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find project workspace model member while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_workspace_model_member_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceMemberSnapshot *out_member)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_member == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->member_count) return UMI_STATUS_NOT_FOUND;
    *out_member = model->members[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by project workspace model member without
 * changing their state.
 */
size_t umi_project_workspace_model_member_count(const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->member_count : 0U;
}

/*
 * Provide the project workspace model set active project operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_set_active_project(
    UmiProjectWorkspaceModel *model, const char *project_id)
{
    UmiProjectWorkspaceMemberSnapshot member;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || project_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_find_project_member(
            model, project_id, &member) != UMI_STATUS_OK ||
        !umi_project_workspace_model_member_is_selectable(model, &member))
        return UMI_STATUS_NOT_FOUND;
    umi_project_workspace_model_copy_text(
        model->active_project_id, sizeof(model->active_project_id), project_id);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the member precedes operation used by this module and its client applications. */
static int member_precedes(const UmiProjectWorkspaceMemberSnapshot *left,
                           const UmiProjectWorkspaceMemberSnapshot *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order != right->order) return left->order < right->order;
    return strcmp(left->project_id, right->project_id) < 0;
}

/*
 * Provide the project workspace model resolve active project operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_model_resolve_active_project(
    const UmiProjectWorkspaceModel *model,
    UmiProjectDescriptorSnapshot *out_project)
{
    const UmiProjectWorkspaceMemberSnapshot *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_project == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (model->active_project_id[0] != '\0') {
        UmiProjectWorkspaceMemberSnapshot selected;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_project_workspace_model_find_project_member(
                model, model->active_project_id, &selected) == UMI_STATUS_OK &&
            umi_project_workspace_model_member_is_selectable(model, &selected))
            return umi_project_descriptor_registry_find(
                umi_project_workspace_descriptor(model->projects),
                selected.project_id, out_project);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->member_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_project_workspace_model_member_is_selectable(
                model, &model->members[index]) &&
            member_precedes(&model->members[index], best))
            best = &model->members[index];
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_project_descriptor_registry_find(
        umi_project_workspace_descriptor(model->projects),
        best->project_id, out_project);
}
