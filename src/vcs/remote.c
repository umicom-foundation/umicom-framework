/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/remote.c
 *
 * PURPOSE:
 *   Implement the remote behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework remote collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/remote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiVcsRemoteList { UmiVcsRemote *items; size_t count; size_t capacity; };
/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiVcsRemoteList *list)
{
    UmiVcsRemote *resized; size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->count < list->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->capacity >= UMI_VCS_MAX_REMOTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 8U : list->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_VCS_MAX_REMOTES) next = UMI_VCS_MAX_REMOTES;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
/*
 * Initialise vcs remote list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_remote_list_create(UmiVcsRemoteList **out_list)
{
    UmiVcsRemoteList *list;
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
    list->capacity = 8U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list; return UMI_STATUS_OK;
}
/* Release or reset state held by vcs remote list so the same storage can be reused safely. */
void umi_vcs_remote_list_destroy(UmiVcsRemoteList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) { free(list->items); free(list); } }
/* Release or reset state held by vcs remote list so the same storage can be reused safely. */
void umi_vcs_remote_list_clear(UmiVcsRemoteList *list)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (list != NULL) list->count = 0U; }
/*
 * Find vcs remote list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsRemote *umi_vcs_remote_list_find(const UmiVcsRemoteList *list, const char *name)
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
 * Provide the vcs remote list upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_remote_list_upsert(UmiVcsRemoteList *list, const UmiVcsRemote *remote)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || remote == NULL || remote->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(list->items[index].name, remote->name) == 0) { list->items[index] = *remote; return UMI_STATUS_OK; }
    }
    { UmiStatus status = ensure_capacity(list); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    list->items[list->count++] = *remote; return UMI_STATUS_OK;
}
/*
 * Read vcs remote list into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_remote_list_parse(UmiVcsRemoteList *list, const char *output)
{
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_remote_list_clear(list); cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[UMI_VCS_URL_CAPACITY + UMI_VCS_NAME_CAPACITY + 32U];
        char name[UMI_VCS_NAME_CAPACITY] = "";
        char url[UMI_VCS_URL_CAPACITY] = "";
        char direction[16] = "";
        UmiVcsRemote remote = {0};
        const UmiVcsRemote *existing;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length); line[length] = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (sscanf(line, "%255s %2047s (%15[^)])", name, url, direction) == 3) {
            existing = umi_vcs_remote_list_find(list, name);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (existing != NULL) remote = *existing;
            (void)snprintf(remote.name, sizeof(remote.name), "%s", name);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(direction, "fetch") == 0) (void)snprintf(remote.fetch_url, sizeof(remote.fetch_url), "%s", url);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(direction, "push") == 0) (void)snprintf(remote.push_url, sizeof(remote.push_url), "%s", url);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_vcs_remote_list_upsert(list, &remote) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
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
 * Return the number of records represented by vcs remote list without changing their
 * state.
 */
size_t umi_vcs_remote_list_count(const UmiVcsRemoteList *list)
{ return list != NULL ? list->count : 0U; }
/*
 * Find vcs remote list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsRemote *umi_vcs_remote_list_at(const UmiVcsRemoteList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
