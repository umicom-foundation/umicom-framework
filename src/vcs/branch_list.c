/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/branch_list.c
 *
 * PURPOSE:
 *   Implement the branch list behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework branch collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/branch_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiVcsBranchList { UmiVcsBranch *items; size_t count; size_t capacity; };
static UmiStatus ensure_capacity(UmiVcsBranchList *list)
{
    UmiVcsBranch *resized; size_t next;
    if (list->count < list->capacity) return UMI_STATUS_OK;
    if (list->capacity >= UMI_VCS_MAX_BRANCHES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 32U : list->capacity * 2U;
    if (next > UMI_VCS_MAX_BRANCHES) next = UMI_VCS_MAX_BRANCHES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
static void parse_tracking(const char *text, int *ahead, int *behind)
{
    const char *value;
    *ahead = 0; *behind = 0;
    value = strstr(text, "ahead ");
    if (value != NULL) *ahead = atoi(value + 6);
    value = strstr(text, "behind ");
    if (value != NULL) *behind = atoi(value + 7);
}
UmiStatus umi_vcs_branch_list_create(UmiVcsBranchList **out_list)
{
    UmiVcsBranchList *list;
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL;
    list = calloc(1U, sizeof(*list));
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 32U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list;
    return UMI_STATUS_OK;
}
void umi_vcs_branch_list_destroy(UmiVcsBranchList *list)
{ if (list != NULL) { free(list->items); free(list); } }
void umi_vcs_branch_list_clear(UmiVcsBranchList *list)
{ if (list != NULL) { list->count = 0U; } }
UmiStatus umi_vcs_branch_list_add(UmiVcsBranchList *list, const UmiVcsBranch *branch)
{
    if (list == NULL || branch == NULL || branch->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_branch_list_find(list, branch->name) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    { UmiStatus status = ensure_capacity(list); if (status != UMI_STATUS_OK) return status; }
    list->items[list->count++] = *branch;
    return UMI_STATUS_OK;
}
UmiStatus umi_vcs_branch_list_parse(UmiVcsBranchList *list, const char *output)
{
    const char *cursor;
    if (list == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_branch_list_clear(list);
    cursor = output;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[UMI_VCS_NAME_CAPACITY * 3U + 128U];
        UmiVcsBranch branch = {0};
        char marker[8] = "";
        char tracking[128] = "";
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length); line[length] = '\0';
        if (sscanf(line, "%7[^\t]\t%255[^\t]\t%255[^\t]\t%127[^\r\n]",
                   marker, branch.name, branch.upstream, tracking) >= 2) {
            branch.current = strcmp(marker, "*") == 0;
            parse_tracking(tracking, &branch.ahead, &branch.behind);
            if (umi_vcs_branch_list_add(list, &branch) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
size_t umi_vcs_branch_list_count(const UmiVcsBranchList *list)
{ return list != NULL ? list->count : 0U; }
const UmiVcsBranch *umi_vcs_branch_list_at(const UmiVcsBranchList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
const UmiVcsBranch *umi_vcs_branch_list_find(const UmiVcsBranchList *list, const char *name)
{
    size_t index;
    if (list == NULL || name == NULL) return NULL;
    for (index = 0U; index < list->count; ++index) if (strcmp(list->items[index].name, name) == 0) return &list->items[index];
    return NULL;
}
const UmiVcsBranch *umi_vcs_branch_list_current(const UmiVcsBranchList *list)
{
    size_t index;
    if (list == NULL) return NULL;
    for (index = 0U; index < list->count; ++index) if (list->items[index].current) return &list->items[index];
    return NULL;
}
