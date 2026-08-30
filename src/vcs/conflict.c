/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/conflict.c
 *
 * PURPOSE:
 *   Implement the conflict behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework merge-conflict projection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/conflict.h"
#include <stdlib.h>
#include <string.h>
struct UmiVcsConflictList { UmiVcsConflict *items; size_t count; size_t capacity; };
static UmiStatus ensure_capacity(UmiVcsConflictList *list)
{
    UmiVcsConflict *resized; size_t next;
    if (list->count < list->capacity) return UMI_STATUS_OK;
    if (list->capacity >= UMI_VCS_MAX_CHANGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 16U : list->capacity * 2U;
    if (next > UMI_VCS_MAX_CHANGES) next = UMI_VCS_MAX_CHANGES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
UmiVcsConflictKind umi_vcs_conflict_kind_from_code(const char code[3])
{
    if (code == NULL) return UMI_VCS_CONFLICT_NONE;
    if (strcmp(code, "AA") == 0) return UMI_VCS_CONFLICT_BOTH_ADDED;
    if (strcmp(code, "UU") == 0) return UMI_VCS_CONFLICT_BOTH_MODIFIED;
    if (strcmp(code, "DD") == 0) return UMI_VCS_CONFLICT_BOTH_DELETED;
    if (strcmp(code, "AU") == 0) return UMI_VCS_CONFLICT_ADDED_BY_US;
    if (strcmp(code, "UA") == 0) return UMI_VCS_CONFLICT_ADDED_BY_THEM;
    if (strcmp(code, "DU") == 0) return UMI_VCS_CONFLICT_DELETED_BY_US;
    if (strcmp(code, "UD") == 0) return UMI_VCS_CONFLICT_DELETED_BY_THEM;
    return UMI_VCS_CONFLICT_UNKNOWN;
}
UmiStatus umi_vcs_conflict_list_create(UmiVcsConflictList **out_list)
{
    UmiVcsConflictList *list;
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL; list = calloc(1U, sizeof(*list));
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 16U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list; return UMI_STATUS_OK;
}
void umi_vcs_conflict_list_destroy(UmiVcsConflictList *list)
{ if (list != NULL) { free(list->items); free(list); } }
void umi_vcs_conflict_list_clear(UmiVcsConflictList *list)
{ if (list != NULL) list->count = 0U; }
UmiStatus umi_vcs_conflict_list_build(UmiVcsConflictList *list, const UmiVcsChangeList *changes)
{
    size_t index;
    if (list == NULL || changes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_conflict_list_clear(list);
    for (index = 0U; index < umi_vcs_change_list_count(changes); ++index) {
        const UmiVcsChange *change = umi_vcs_change_list_at(changes, index);
        UmiVcsConflict *conflict;
        if (change == NULL) continue;
        if (!((change->index_state == UMI_VCS_CHANGE_CONFLICTED) ||
              (change->worktree_state == UMI_VCS_CHANGE_CONFLICTED) ||
              (change->index_state == UMI_VCS_CHANGE_ADDED && change->worktree_state == UMI_VCS_CHANGE_ADDED) ||
              (change->index_state == UMI_VCS_CHANGE_DELETED && change->worktree_state == UMI_VCS_CHANGE_DELETED))) continue;
        { UmiStatus status = ensure_capacity(list); if (status != UMI_STATUS_OK) return status; }
        conflict = &list->items[list->count++];
        (void)memset(conflict, 0, sizeof(*conflict));
        (void)memcpy(conflict->path, change->path, strlen(change->path) + 1U);
        conflict->index_code[0] = change->index_state == UMI_VCS_CHANGE_CONFLICTED ? 'U' :
            (change->index_state == UMI_VCS_CHANGE_ADDED ? 'A' :
             (change->index_state == UMI_VCS_CHANGE_DELETED ? 'D' : '?'));
        conflict->index_code[1] = change->worktree_state == UMI_VCS_CHANGE_CONFLICTED ? 'U' :
            (change->worktree_state == UMI_VCS_CHANGE_ADDED ? 'A' :
             (change->worktree_state == UMI_VCS_CHANGE_DELETED ? 'D' : '?'));
        conflict->index_code[2] = '\0';
        conflict->kind = umi_vcs_conflict_kind_from_code(conflict->index_code);
    }
    return UMI_STATUS_OK;
}
size_t umi_vcs_conflict_list_count(const UmiVcsConflictList *list)
{ return list != NULL ? list->count : 0U; }
const UmiVcsConflict *umi_vcs_conflict_list_at(const UmiVcsConflictList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
const UmiVcsConflict *umi_vcs_conflict_list_find(const UmiVcsConflictList *list, const char *path)
{
    size_t index;
    if (list == NULL || path == NULL) return NULL;
    for (index = 0U; index < list->count; ++index) if (strcmp(list->items[index].path, path) == 0) return &list->items[index];
    return NULL;
}
