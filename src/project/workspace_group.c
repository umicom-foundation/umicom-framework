/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_group.c
 * PURPOSE: Implement project groups and stable topological project ordering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_group.h"
#include "workspace_model_internal.h"

#include <stdint.h>
#include <string.h>

size_t umi_project_workspace_model_find_group_index(
    const UmiProjectWorkspaceModel *model, const char *group_id)
{
    size_t index;
    if (model == NULL || group_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->group_count; ++index)
        if (strcmp(model->groups[index].id, group_id) == 0) return index;
    return SIZE_MAX;
}

UmiStatus umi_project_workspace_model_upsert_group(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceGroupSnapshot *group)
{
    size_t index;
    size_t parent_index;
    size_t visited;
    if (model == NULL || group == NULL || group->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (group->parent_group_id[0] != '\0' &&
        strcmp(group->id, group->parent_group_id) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    parent_index = umi_project_workspace_model_find_group_index(
        model, group->parent_group_id);
    visited = 0U;
    while (parent_index != SIZE_MAX && visited < model->group_count) {
        if (strcmp(model->groups[parent_index].id, group->id) == 0)
            return UMI_STATUS_INVALID_STATE;
        if (model->groups[parent_index].parent_group_id[0] == '\0') break;
        parent_index = umi_project_workspace_model_find_group_index(
            model, model->groups[parent_index].parent_group_id);
        visited += 1U;
    }
    index = umi_project_workspace_model_find_group_index(model, group->id);
    if (index != SIZE_MAX &&
        strcmp(model->groups[index].name, group->name) == 0 &&
        strcmp(model->groups[index].parent_group_id,
               group->parent_group_id) == 0 &&
        model->groups[index].order == group->order &&
        model->groups[index].enabled == group->enabled)
        return UMI_STATUS_OK;
    if (index == SIZE_MAX) {
        if (model->group_count >= UMI_PROJECT_WORKSPACE_GROUP_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = model->group_count++;
    }
    model->groups[index] = *group;
    model->groups[index].struct_size = (uint32_t)sizeof(model->groups[index]);
    model->groups[index].api_version = UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    model->groups[index].id[127U] = '\0';
    model->groups[index].name[255U] = '\0';
    model->groups[index].parent_group_id[127U] = '\0';
    model->groups[index].revision = ++model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_workspace_model_find_group(
    const UmiProjectWorkspaceModel *model, const char *group_id,
    UmiProjectWorkspaceGroupSnapshot *out_group)
{
    size_t index;
    if (model == NULL || group_id == NULL || out_group == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = umi_project_workspace_model_find_group_index(model, group_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_group = model->groups[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_project_workspace_model_group_at(
    const UmiProjectWorkspaceModel *model, size_t index,
    UmiProjectWorkspaceGroupSnapshot *out_group)
{
    if (model == NULL || out_group == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = model->groups[index];
    return UMI_STATUS_OK;
}

size_t umi_project_workspace_model_group_count(const UmiProjectWorkspaceModel *model)
{
    return model != NULL ? model->group_count : 0U;
}

static size_t selectable_member_indices(
    const UmiProjectWorkspaceModel *model,
    size_t indices[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY])
{
    size_t count = 0U;
    size_t index;
    for (index = 0U; index < model->member_count; ++index)
        if (umi_project_workspace_model_member_is_selectable(
                model, &model->members[index]))
            indices[count++] = index;
    return count;
}

static size_t project_position(
    const UmiProjectWorkspaceModel *model,
    const size_t *indices,
    size_t count,
    const char *project_id)
{
    size_t index;
    for (index = 0U; index < count; ++index)
        if (strcmp(model->members[indices[index]].project_id, project_id) == 0)
            return index;
    return SIZE_MAX;
}

static int candidate_precedes(const UmiProjectWorkspaceMemberSnapshot *left,
                              const UmiProjectWorkspaceMemberSnapshot *right)
{
    if (right == NULL) return 1;
    if (left->order != right->order) return left->order < right->order;
    return strcmp(left->project_id, right->project_id) < 0;
}

UmiStatus umi_project_workspace_model_resolve_build_order(
    const UmiProjectWorkspaceModel *model,
    UmiProjectWorkspaceOrderSnapshot *out_order)
{
    size_t indices[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY];
    size_t indegree[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY] = {0};
    int emitted[UMI_PROJECT_WORKSPACE_MEMBER_CAPACITY] = {0};
    size_t count;
    size_t reference_index;
    size_t index;
    if (model == NULL || out_order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_order, 0, sizeof(*out_order));
    out_order->struct_size = (uint32_t)sizeof(*out_order);
    out_order->api_version = UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    count = selectable_member_indices(model, indices);

    for (reference_index = 0U;
         reference_index < umi_project_reference_registry_count(
             umi_project_workspace_reference(model->projects));
         ++reference_index) {
        UmiProjectReferenceSnapshot reference;
        size_t source;
        size_t target;
        if (umi_project_reference_registry_at(
                umi_project_workspace_reference(model->projects),
                reference_index, &reference) != UMI_STATUS_OK) continue;
        source = project_position(model, indices, count, reference.project_id);
        target = project_position(model, indices, count,
                                  reference.target_project_id);
        if (source == SIZE_MAX || target == SIZE_MAX) {
            if (reference.required != 0)
                out_order->unresolved_reference_count += 1U;
            continue;
        }
        indegree[source] += 1U;
    }

    while (out_order->project_count < count) {
        size_t best = SIZE_MAX;
        const UmiProjectWorkspaceMemberSnapshot *best_member = NULL;
        for (index = 0U; index < count; ++index) {
            const UmiProjectWorkspaceMemberSnapshot *candidate;
            if (emitted[index] || indegree[index] != 0U) continue;
            candidate = &model->members[indices[index]];
            if (candidate_precedes(candidate, best_member)) {
                best = index;
                best_member = candidate;
            }
        }
        if (best == SIZE_MAX) {
            out_order->has_cycle = 1;
            return UMI_STATUS_INVALID_STATE;
        }
        umi_project_workspace_model_copy_text(
            out_order->project_ids[out_order->project_count++], 128U,
            best_member->project_id);
        emitted[best] = 1;

        for (reference_index = 0U;
             reference_index < umi_project_reference_registry_count(
                 umi_project_workspace_reference(model->projects));
             ++reference_index) {
            UmiProjectReferenceSnapshot reference;
            size_t source;
            if (umi_project_reference_registry_at(
                    umi_project_workspace_reference(model->projects),
                    reference_index, &reference) != UMI_STATUS_OK) continue;
            if (strcmp(reference.target_project_id,
                       best_member->project_id) != 0) continue;
            source = project_position(model, indices, count,
                                      reference.project_id);
            if (source != SIZE_MAX && indegree[source] > 0U)
                indegree[source] -= 1U;
        }
    }
    return UMI_STATUS_OK;
}
