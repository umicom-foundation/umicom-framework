/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/source_bookmark.c
 *
 * PURPOSE:
 *   Implement owned source bookmarks and predictable per-document traversal.
 *   Disabled bookmarks are retained for persistence but skipped by navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/source_bookmark.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSourceBookmarkRegistry {
    UmiEditorSourceBookmark *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_bookmark(const UmiEditorSourceBookmark *bookmark)
{
    if (bookmark == NULL ||
        bookmark->struct_size != (uint32_t)sizeof(*bookmark) ||
        bookmark->api_version != UMI_EDITOR_SOURCE_BOOKMARK_API_VERSION ||
        !text_terminated(bookmark->id, sizeof(bookmark->id)) ||
        bookmark->id[0] == '\0' ||
        !text_terminated(bookmark->group, sizeof(bookmark->group)) ||
        !text_terminated(bookmark->description,
                         sizeof(bookmark->description))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_source_location_validate(&bookmark->location);
}

static UmiStatus reserve_items(UmiEditorSourceBookmarkRegistry *registry,
                               size_t required)
{
    size_t capacity;
    UmiEditorSourceBookmark *replacement;

    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 32U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSourceBookmark *)realloc(
        registry->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_index(const UmiEditorSourceBookmarkRegistry *registry,
                         const char *id)
{
    size_t index;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_source_bookmark_registry_create(
    UmiEditorSourceBookmarkRegistry **out_registry)
{
    UmiEditorSourceBookmarkRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorSourceBookmarkRegistry *)calloc(1U,
                                                          sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_editor_source_bookmark_registry_destroy(
    UmiEditorSourceBookmarkRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

UmiStatus umi_editor_source_bookmark_registry_upsert(
    UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceBookmark *bookmark)
{
    size_t index;
    UmiEditorSourceBookmark stored;
    UmiStatus status;

    if (registry == NULL || validate_bookmark(bookmark) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, bookmark->id);
    if (index == SIZE_MAX) {
        status = reserve_items(registry, registry->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    stored = *bookmark;
    registry->revision = next_revision(registry->revision);
    stored.revision = registry->revision;
    registry->items[index] = stored;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_source_bookmark_registry_remove(
    UmiEditorSourceBookmarkRegistry *registry,
    const char *id)
{
    size_t index;

    if (registry == NULL || id == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(registry->items + index,
                      registry->items + index + 1U,
                      (registry->count - index - 1U) *
                          sizeof(*registry->items));
    }
    registry->count -= 1U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_source_bookmark_registry_find(
    const UmiEditorSourceBookmarkRegistry *registry,
    const char *id,
    UmiEditorSourceBookmark *out_bookmark)
{
    size_t index;

    if (registry == NULL || id == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_source_bookmark_registry_at(
    const UmiEditorSourceBookmarkRegistry *registry,
    size_t index,
    UmiEditorSourceBookmark *out_bookmark)
{
    if (registry == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = registry->items[index];
    return UMI_STATUS_OK;
}

static int is_candidate(const UmiEditorSourceBookmark *bookmark,
                        const UmiEditorSourceLocation *from,
                        const char *group)
{
    return bookmark->enabled &&
           strcmp(bookmark->location.uri, from->uri) == 0 &&
           (group == NULL || group[0] == '\0' ||
            strcmp(bookmark->group, group) == 0);
}

static UmiStatus traverse(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int forward,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark)
{
    const UmiEditorSourceBookmark *best = NULL;
    const UmiEditorSourceBookmark *wrapped = NULL;
    size_t index;

    if (registry == NULL || out_bookmark == NULL ||
        umi_editor_source_location_validate(from) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        const UmiEditorSourceBookmark *candidate = &registry->items[index];
        int order;

        if (!is_candidate(candidate, from, group)) continue;
        order = umi_editor_source_location_compare(&candidate->location, from);
        if (forward && order > 0 &&
            (best == NULL || umi_editor_source_location_compare(
                &candidate->location, &best->location) < 0)) {
            best = candidate;
        } else if (!forward && order < 0 &&
                   (best == NULL || umi_editor_source_location_compare(
                       &candidate->location, &best->location) > 0)) {
            best = candidate;
        }
        if (forward &&
            (wrapped == NULL || umi_editor_source_location_compare(
                &candidate->location, &wrapped->location) < 0)) {
            wrapped = candidate;
        } else if (!forward &&
                   (wrapped == NULL || umi_editor_source_location_compare(
                       &candidate->location, &wrapped->location) > 0)) {
            wrapped = candidate;
        }
    }
    if (best == NULL && wrap) best = wrapped;
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = *best;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_source_bookmark_registry_next(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark)
{
    return traverse(registry, from, group, 1, wrap, out_bookmark);
}

UmiStatus umi_editor_source_bookmark_registry_previous(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark)
{
    return traverse(registry, from, group, 0, wrap, out_bookmark);
}

size_t umi_editor_source_bookmark_registry_count(
    const UmiEditorSourceBookmarkRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_editor_source_bookmark_registry_revision(
    const UmiEditorSourceBookmarkRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
