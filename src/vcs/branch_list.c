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
/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiVcsBranchList *list)
{
    UmiVcsBranch *resized; size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->count < list->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->capacity >= UMI_VCS_MAX_BRANCHES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 32U : list->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_VCS_MAX_BRANCHES) next = UMI_VCS_MAX_BRANCHES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
/* Provide the parse tracking operation used by this module and its client applications. */
static void parse_tracking(const char *text, int *ahead, int *behind)
{
    const char *value;
    *ahead = 0; *behind = 0;
    value = strstr(text, "ahead ");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) *ahead = atoi(value + 6);
    value = strstr(text, "behind ");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) *behind = atoi(value + 7);
}
/*
 * Initialise vcs branch list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_branch_list_create(UmiVcsBranchList **out_list)
{
    UmiVcsBranchList *list;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL;
    list = calloc(1U, sizeof(*list));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 32U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list;
    return UMI_STATUS_OK;
}
/* Release or reset state held by vcs branch list so the same storage can be reused safely. */
void umi_vcs_branch_list_destroy(UmiVcsBranchList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) { free(list->items); free(list); } }
/* Release or reset state held by vcs branch list so the same storage can be reused safely. */
void umi_vcs_branch_list_clear(UmiVcsBranchList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) { list->count = 0U; } }
/* Add vcs branch list only after its inputs and available capacity have been checked. */
UmiStatus umi_vcs_branch_list_add(UmiVcsBranchList *list, const UmiVcsBranch *branch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || branch == NULL || branch->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_branch_list_find(list, branch->name) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    { UmiStatus status = ensure_capacity(list); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    list->items[list->count++] = *branch;
    return UMI_STATUS_OK;
}
/*
 * Read vcs branch list into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_branch_list_parse(UmiVcsBranchList *list, const char *output)
{
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_branch_list_clear(list);
    cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[UMI_VCS_NAME_CAPACITY * 3U + 128U];
        UmiVcsBranch branch = {0};
        char marker[8] = "";
        char tracking[128] = "";
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length); line[length] = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (sscanf(line, "%7[^\t]\t%255[^\t]\t%255[^\t]\t%127[^\r\n]",
                   marker, branch.name, branch.upstream, tracking) >= 2) {
            branch.current = strcmp(marker, "*") == 0;
            parse_tracking(tracking, &branch.ahead, &branch.behind);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_vcs_branch_list_add(list, &branch) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by vcs branch list without changing their
 * state.
 */
size_t umi_vcs_branch_list_count(const UmiVcsBranchList *list)
{ return list != NULL ? list->count : 0U; }
/*
 * Find vcs branch list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsBranch *umi_vcs_branch_list_at(const UmiVcsBranchList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
/*
 * Find vcs branch list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsBranch *umi_vcs_branch_list_find(const UmiVcsBranchList *list, const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(list->items[index].name, name) == 0) return &list->items[index];
    return NULL;
}
/*
 * Provide the vcs branch list current operation used by this module and its client
 * applications.
 */
const UmiVcsBranch *umi_vcs_branch_list_current(const UmiVcsBranchList *list)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (list->items[index].current) return &list->items[index];
    return NULL;
}
