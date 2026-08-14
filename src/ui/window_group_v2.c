/* Umicom Framework | Linked workspace windows v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/window_group_v2.h"
#include <stdio.h>
#include <string.h>

static UmiUiWindowGroupV2 *find_mutable(UmiUiWindowGroupStoreV2 *store,const char *group_id)
{ size_t index; if (store == NULL || group_id == NULL) return NULL; for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].group_id,group_id) == 0) return &store->items[index]; return NULL; }
UmiStatus umi_ui_window_group_v2_define(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *colour_token,UmiUiWindowContextKind context_kind)
{
    UmiUiWindowGroupV2 *group; int first; int second;
    if (store == NULL || group_id == NULL || colour_token == NULL || group_id[0] == '\0' || colour_token[0] == '\0' || context_kind < UMI_UI_WINDOW_CONTEXT_GENERIC || context_kind > UMI_UI_WINDOW_CONTEXT_DIAGNOSTIC) return UMI_STATUS_INVALID_ARGUMENT;
    if (find_mutable(store,group_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_UI_WINDOW_GROUP_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    group = &store->items[store->count]; (void)memset(group,0,sizeof(*group));
    first = snprintf(group->group_id,sizeof(group->group_id),"%s",group_id); second = snprintf(group->colour_token,sizeof(group->colour_token),"%s",colour_token);
    if (first < 0 || second < 0 || (size_t)first >= sizeof(group->group_id) || (size_t)second >= sizeof(group->colour_token)) return UMI_STATUS_CAPACITY_EXCEEDED;
    group->context_kind = context_kind; group->revision = 1U; store->count += 1U; store->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_window_group_v2_join(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *window_id,UmiUiWindowGroupRole role)
{
    UmiUiWindowGroupV2 *group; size_t index; int length;
    if (store == NULL || group_id == NULL || window_id == NULL || window_id[0] == '\0' || role < UMI_UI_WINDOW_GROUP_SOURCE || role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) return UMI_STATUS_INVALID_ARGUMENT;
    group = find_mutable(store,group_id); if (group == NULL) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,window_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (group->member_count >= UMI_UI_WINDOW_GROUP_MAX_MEMBERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(group->members[group->member_count].window_id,sizeof(group->members[0].window_id),"%s",window_id); if (length < 0 || (size_t)length >= sizeof(group->members[0].window_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    group->members[group->member_count].role = role; group->member_count += 1U; group->revision += 1U; store->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_window_group_v2_leave(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *window_id)
{
    UmiUiWindowGroupV2 *group; size_t index;
    if (store == NULL || group_id == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    group = find_mutable(store,group_id); if (group == NULL) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,window_id) == 0) { (void)memmove(&group->members[index],&group->members[index + 1U],(group->member_count - index - 1U) * sizeof(group->members[0])); group->member_count -= 1U; group->revision += 1U; store->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
const UmiUiWindowGroupV2 *umi_ui_window_group_v2_find(const UmiUiWindowGroupStoreV2 *store,const char *group_id) { return find_mutable((UmiUiWindowGroupStoreV2 *)(void *)store,group_id); }
size_t umi_ui_window_group_v2_route(const UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *source_window_id,const char **out_window_ids,size_t capacity)
{
    const UmiUiWindowGroupV2 *group = umi_ui_window_group_v2_find(store,group_id); size_t index; size_t found = 0U; bool source_allowed = false;
    if (group == NULL || source_window_id == NULL || out_window_ids == NULL) return 0U;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,source_window_id) == 0 && group->members[index].role != UMI_UI_WINDOW_GROUP_DESTINATION) source_allowed = true;
    if (!source_allowed) return 0U;
    for (index = 0U; index < group->member_count; ++index) if (strcmp(group->members[index].window_id,source_window_id) != 0 && group->members[index].role != UMI_UI_WINDOW_GROUP_SOURCE) { if (found < capacity) out_window_ids[found] = group->members[index].window_id; found += 1U; }
    return found;
}
