/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/tag.c
 *
 * PURPOSE:
 *   Implement the tag behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework tag collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/tag.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiVcsTagList { UmiVcsTag *items; size_t count; size_t capacity; };
static UmiStatus ensure_capacity(UmiVcsTagList *list)
{
    UmiVcsTag *resized; size_t next;
    if (list->count < list->capacity) return UMI_STATUS_OK;
    if (list->capacity >= UMI_VCS_MAX_TAGS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = list->capacity == 0U ? 32U : list->capacity * 2U;
    if (next > UMI_VCS_MAX_TAGS) next = UMI_VCS_MAX_TAGS;
    resized = realloc(list->items, next * sizeof(list->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized; list->capacity = next; return UMI_STATUS_OK;
}
UmiStatus umi_vcs_tag_list_create(UmiVcsTagList **out_list)
{
    UmiVcsTagList *list;
    if (out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_list = NULL; list = calloc(1U, sizeof(*list));
    if (list == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->capacity = 32U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    if (list->items == NULL) { free(list); return UMI_STATUS_OUT_OF_MEMORY; }
    *out_list = list; return UMI_STATUS_OK;
}
void umi_vcs_tag_list_destroy(UmiVcsTagList *list)
{ if (list != NULL) { free(list->items); free(list); } }
void umi_vcs_tag_list_clear(UmiVcsTagList *list)
{ if (list != NULL) list->count = 0U; }
const UmiVcsTag *umi_vcs_tag_list_find(const UmiVcsTagList *list, const char *name)
{
    size_t index;
    if (list == NULL || name == NULL) return NULL;
    for (index = 0U; index < list->count; ++index) if (strcmp(list->items[index].name, name) == 0) return &list->items[index];
    return NULL;
}
UmiStatus umi_vcs_tag_list_add(UmiVcsTagList *list, const UmiVcsTag *tag)
{
    if (list == NULL || tag == NULL || tag->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_tag_list_find(list, tag->name) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    { UmiStatus status = ensure_capacity(list); if (status != UMI_STATUS_OK) return status; }
    list->items[list->count++] = *tag; return UMI_STATUS_OK;
}
UmiStatus umi_vcs_tag_list_parse(UmiVcsTagList *list, const char *output)
{
    const char *cursor;
    if (list == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_tag_list_clear(list); cursor = output;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[UMI_VCS_MESSAGE_CAPACITY + UMI_VCS_NAME_CAPACITY + UMI_VCS_ID_CAPACITY + 8U];
        UmiVcsTag tag = {0};
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length); line[length] = '\0';
        if (sscanf(line, "%255[^\t]\t%127[^\t]\t%1023[^\r\n]", tag.name, tag.target_id, tag.subject) >= 2) {
            UmiStatus status = umi_vcs_tag_list_add(list, &tag);
            if (status != UMI_STATUS_OK) return status;
        }
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
size_t umi_vcs_tag_list_count(const UmiVcsTagList *list)
{ return list != NULL ? list->count : 0U; }
const UmiVcsTag *umi_vcs_tag_list_at(const UmiVcsTagList *list, size_t index)
{ return list != NULL && index < list->count ? &list->items[index] : NULL; }
