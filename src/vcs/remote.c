/* Umicom Framework remote collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/remote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiVcsRemoteList { UmiVcsRemote *items; size_t count; size_t capacity; };
static UmiStatus ensure_capacity(UmiVcsRemoteList *list)
{
    UmiVcsRemote *resized; size_t next;
    if (list->count < list->capacity) return UMI_STATUS_OK;
    if (list->capacity >= UMI_VCS_MAX_REMOTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 8U : list->capacity * 2U;
    if (next > UMI_VCS_MAX_REMOTES) next = UMI_VCS_MAX_REMOTES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
UmiStatus umi_vcs_remote_list_create(UmiVcsRemoteList **out_list)
{
    UmiVcsRemoteList *list;
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL; list = calloc(1U, sizeof(*list));
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 8U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list; return UMI_STATUS_OK;
}
void umi_vcs_remote_list_destroy(UmiVcsRemoteList *list)
{ if (list != NULL) { free(list->items); free(list); } }
void umi_vcs_remote_list_clear(UmiVcsRemoteList *list)
{ if (list != NULL) list->count = 0U; }
const UmiVcsRemote *umi_vcs_remote_list_find(const UmiVcsRemoteList *list, const char *name)
{
    size_t index;
    if (list == NULL || name == NULL) return NULL;
    for (index = 0U; index < list->count; ++index) if (strcmp(list->items[index].name, name) == 0) return &list->items[index];
    return NULL;
}
UmiStatus umi_vcs_remote_list_upsert(UmiVcsRemoteList *list, const UmiVcsRemote *remote)
{
    size_t index;
    if (list == NULL || remote == NULL || remote->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < list->count; ++index) {
        if (strcmp(list->items[index].name, remote->name) == 0) { list->items[index] = *remote; return UMI_STATUS_OK; }
    }
    { UmiStatus status = ensure_capacity(list); if (status != UMI_STATUS_OK) return status; }
    list->items[list->count++] = *remote; return UMI_STATUS_OK;
}
UmiStatus umi_vcs_remote_list_parse(UmiVcsRemoteList *list, const char *output)
{
    const char *cursor;
    if (list == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_remote_list_clear(list); cursor = output;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[UMI_VCS_URL_CAPACITY + UMI_VCS_NAME_CAPACITY + 32U];
        char name[UMI_VCS_NAME_CAPACITY] = "";
        char url[UMI_VCS_URL_CAPACITY] = "";
        char direction[16] = "";
        UmiVcsRemote remote = {0};
        const UmiVcsRemote *existing;
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length); line[length] = '\0';
        if (sscanf(line, "%255s %2047s (%15[^)])", name, url, direction) == 3) {
            existing = umi_vcs_remote_list_find(list, name);
            if (existing != NULL) remote = *existing;
            (void)snprintf(remote.name, sizeof(remote.name), "%s", name);
            if (strcmp(direction, "fetch") == 0) (void)snprintf(remote.fetch_url, sizeof(remote.fetch_url), "%s", url);
            if (strcmp(direction, "push") == 0) (void)snprintf(remote.push_url, sizeof(remote.push_url), "%s", url);
            if (umi_vcs_remote_list_upsert(list, &remote) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
size_t umi_vcs_remote_list_count(const UmiVcsRemoteList *list)
{ return list != NULL ? list->count : 0U; }
const UmiVcsRemote *umi_vcs_remote_list_at(const UmiVcsRemoteList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
