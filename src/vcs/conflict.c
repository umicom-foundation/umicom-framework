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
/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiVcsConflictList *list)
{
    UmiVcsConflict *resized; size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->count < list->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->capacity >= UMI_VCS_MAX_CHANGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 16U : list->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_VCS_MAX_CHANGES) next = UMI_VCS_MAX_CHANGES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
/*
 * Provide the vcs conflict kind from code operation used by this module and its client
 * applications.
 */
UmiVcsConflictKind umi_vcs_conflict_kind_from_code(const char code[3])
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (code == NULL) return UMI_VCS_CONFLICT_NONE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "AA") == 0) return UMI_VCS_CONFLICT_BOTH_ADDED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "UU") == 0) return UMI_VCS_CONFLICT_BOTH_MODIFIED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "DD") == 0) return UMI_VCS_CONFLICT_BOTH_DELETED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "AU") == 0) return UMI_VCS_CONFLICT_ADDED_BY_US;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "UA") == 0) return UMI_VCS_CONFLICT_ADDED_BY_THEM;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "DU") == 0) return UMI_VCS_CONFLICT_DELETED_BY_US;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(code, "UD") == 0) return UMI_VCS_CONFLICT_DELETED_BY_THEM;
    return UMI_VCS_CONFLICT_UNKNOWN;
}
/*
 * Initialise vcs conflict list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_conflict_list_create(UmiVcsConflictList **out_list)
{
    UmiVcsConflictList *list;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL; list = calloc(1U, sizeof(*list));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 16U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by vcs conflict list so the same storage can be reused
 * safely.
 */
void umi_vcs_conflict_list_destroy(UmiVcsConflictList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) { free(list->items); free(list); } }
/*
 * Release or reset state held by vcs conflict list so the same storage can be reused
 * safely.
 */
void umi_vcs_conflict_list_clear(UmiVcsConflictList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) list->count = 0U; }
/*
 * Provide the vcs conflict list build operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_conflict_list_build(UmiVcsConflictList *list, const UmiVcsChangeList *changes)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || changes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_conflict_list_clear(list);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_vcs_change_list_count(changes); ++index) {
        const UmiVcsChange *change = umi_vcs_change_list_at(changes, index);
        UmiVcsConflict *conflict;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (change == NULL) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!((change->index_state == UMI_VCS_CHANGE_CONFLICTED) ||
              (change->worktree_state == UMI_VCS_CHANGE_CONFLICTED) ||
              (change->index_state == UMI_VCS_CHANGE_ADDED && change->worktree_state == UMI_VCS_CHANGE_ADDED) ||
              (change->index_state == UMI_VCS_CHANGE_DELETED && change->worktree_state == UMI_VCS_CHANGE_DELETED))) continue;
        { UmiStatus status = ensure_capacity(list); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
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
/*
 * Return the number of records represented by vcs conflict list without changing their
 * state.
 */
size_t umi_vcs_conflict_list_count(const UmiVcsConflictList *list)
{ return list != NULL ? list->count : 0U; }
/*
 * Find vcs conflict list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsConflict *umi_vcs_conflict_list_at(const UmiVcsConflictList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
/*
 * Find vcs conflict list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsConflict *umi_vcs_conflict_list_find(const UmiVcsConflictList *list, const char *path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || path == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(list->items[index].path, path) == 0) return &list->items[index];
    return NULL;
}
