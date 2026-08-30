/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/window_group.c
 *
 * PURPOSE:
 *   Implement the window group behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/window_group.h"
#include <stdio.h>
#include <string.h>

static UmiUiWindowGroup *find_mutable(UmiUiWindowGroupStore *store,const char *group_id)
{ size_t index; if (store == NULL || group_id == NULL) return NULL; for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].group_id,group_id) == 0) return &store->items[index]; return NULL; }

static bool remove_member(UmiUiWindowGroup *group, const char *window_id)
{
    size_t index;
    if (group == NULL || window_id == NULL) return false;
    for (index = 0U; index < group->member_count; ++index) {
        if (strcmp(group->members[index].window_id, window_id) == 0) {
            (void)memmove(
                &group->members[index],
                &group->members[index + 1U],
                (group->member_count - index - 1U) *
                    sizeof(group->members[0]));
            group->member_count -= 1U;
            group->revision += 1U;
            return true;
        }
    }
    return false;
}
UmiStatus umi_ui_window_group_define(UmiUiWindowGroupStore *store,const char *group_id,const char *colour_token,UmiUiWindowContextKind context_kind)
{
    UmiUiWindowGroup *group; int first; int second;
    if (store == NULL || group_id == NULL || colour_token == NULL || group_id[0] == '\0' || colour_token[0] == '\0' || context_kind < UMI_UI_WINDOW_CONTEXT_GENERIC || context_kind > UMI_UI_WINDOW_CONTEXT_TIMELINE) return UMI_STATUS_INVALID_ARGUMENT;
    if (find_mutable(store,group_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_UI_WINDOW_GROUP_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    group = &store->items[store->count]; (void)memset(group,0,sizeof(*group));
    first = snprintf(group->group_id,sizeof(group->group_id),"%s",group_id); second = snprintf(group->colour_token,sizeof(group->colour_token),"%s",colour_token);
    if (first < 0 || second < 0 || (size_t)first >= sizeof(group->group_id) || (size_t)second >= sizeof(group->colour_token)) return UMI_STATUS_CAPACITY_EXCEEDED;
    group->context_kind = context_kind; group->revision = 1U; store->count += 1U; store->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_window_group_join(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id,UmiUiWindowGroupRole role)
{
    UmiUiWindowGroup *group; size_t index; int length;
    if (store == NULL || group_id == NULL || window_id == NULL || window_id[0] == '\0' || role < UMI_UI_WINDOW_GROUP_SOURCE || role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) return UMI_STATUS_INVALID_ARGUMENT;
    group = find_mutable(store,group_id); if (group == NULL) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,window_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (group->member_count >= UMI_UI_WINDOW_GROUP_MAX_MEMBERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(group->members[group->member_count].window_id,sizeof(group->members[0].window_id),"%s",window_id); if (length < 0 || (size_t)length >= sizeof(group->members[0].window_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    group->members[group->member_count].role = role; group->member_count += 1U; group->revision += 1U; store->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_window_group_leave(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id)
{
    UmiUiWindowGroup *group;
    if (store == NULL || group_id == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    group = find_mutable(store,group_id); if (group == NULL) return UMI_STATUS_NOT_FOUND;
    if (remove_member(group, window_id)) {
        store->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_ui_window_group_assign(
    UmiUiWindowGroupStore *store,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role)
{
    UmiUiWindowGroup *target;
    UmiUiWindowGroupMember *target_member = NULL;
    size_t group_index;
    size_t member_index;
    int written;

    if (store == NULL || group_id == NULL || window_id == NULL ||
        window_id[0] == '\0' || role < UMI_UI_WINDOW_GROUP_SOURCE ||
        role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(window_id) >= sizeof(store->items[0].members[0].window_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    target = find_mutable(store, group_id);
    if (target == NULL) return UMI_STATUS_NOT_FOUND;
    for (member_index = 0U; member_index < target->member_count;
         ++member_index) {
        if (strcmp(target->members[member_index].window_id, window_id) == 0) {
            target_member = &target->members[member_index];
            break;
        }
    }
    if (target_member == NULL &&
        target->member_count >= UMI_UI_WINDOW_GROUP_MAX_MEMBERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* A window belongs to one linked context group. Remove older assignments
     * before recording the new source/destination role. */
    for (group_index = 0U; group_index < store->count; ++group_index) {
        UmiUiWindowGroup *group = &store->items[group_index];
        if (group != target && remove_member(group, window_id)) {
            store->revision += 1U;
        }
    }
    if (target_member != NULL) {
        target_member->role = role;
    } else {
        target_member = &target->members[target->member_count];
        (void)memset(target_member, 0, sizeof(*target_member));
        written = snprintf(
            target_member->window_id,
            sizeof(target_member->window_id),
            "%s",
            window_id);
        if (written < 0 ||
            (size_t)written >= sizeof(target_member->window_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        target_member->role = role;
        target->member_count += 1U;
    }
    target->revision += 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_window_group_unassign(
    UmiUiWindowGroupStore *store,
    const char *window_id)
{
    size_t index;
    bool removed = false;
    if (store == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) {
        if (remove_member(&store->items[index], window_id)) removed = true;
    }
    if (!removed) return UMI_STATUS_NOT_FOUND;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiUiWindowGroup *umi_ui_window_group_find(const UmiUiWindowGroupStore *store,const char *group_id) { return find_mutable((UmiUiWindowGroupStore *)(void *)store,group_id); }

const UmiUiWindowGroup *umi_ui_window_group_for_window(
    const UmiUiWindowGroupStore *store,
    const char *window_id,
    UmiUiWindowGroupRole *out_role)
{
    size_t group_index;
    size_t member_index;
    if (store == NULL || window_id == NULL) return NULL;
    for (group_index = 0U; group_index < store->count; ++group_index) {
        const UmiUiWindowGroup *group = &store->items[group_index];
        for (member_index = 0U; member_index < group->member_count;
             ++member_index) {
            if (strcmp(group->members[member_index].window_id, window_id) == 0) {
                if (out_role != NULL) {
                    *out_role = group->members[member_index].role;
                }
                return group;
            }
        }
    }
    return NULL;
}
size_t umi_ui_window_group_route(const UmiUiWindowGroupStore *store,const char *group_id,const char *source_window_id,const char **out_window_ids,size_t capacity)
{
    const UmiUiWindowGroup *group = umi_ui_window_group_find(store,group_id); size_t index; size_t found = 0U; bool source_allowed = false;
    if (group == NULL || source_window_id == NULL || out_window_ids == NULL) return 0U;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,source_window_id) == 0 && group->members[index].role != UMI_UI_WINDOW_GROUP_DESTINATION) source_allowed = true;
    if (!source_allowed) return 0U;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,source_window_id) != 0 && group->members[index].role != UMI_UI_WINDOW_GROUP_SOURCE) { if (found < capacity) out_window_ids[found] = group->members[index].window_id; found += 1U; }
    return found;
}
