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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the text terminated operation used by this module and its client applications. */
static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate bookmark operation used by this module and its client applications. */
static UmiStatus validate_bookmark(const UmiEditorSourceBookmark *bookmark)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorSourceBookmarkRegistry *registry,
                               size_t required)
{
    size_t capacity;
    UmiEditorSourceBookmark *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 32U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSourceBookmark *)realloc(
        registry->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorSourceBookmarkRegistry *registry,
                         const char *id)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise editor source bookmark registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_source_bookmark_registry_create(
    UmiEditorSourceBookmarkRegistry **out_registry)
{
    UmiEditorSourceBookmarkRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorSourceBookmarkRegistry *)calloc(1U,
                                                          sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor source bookmark registry so the same storage can
 * be reused safely.
 */
void umi_editor_source_bookmark_registry_destroy(
    UmiEditorSourceBookmarkRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

/*
 * Provide the editor source bookmark registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_upsert(
    UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceBookmark *bookmark)
{
    size_t index;
    UmiEditorSourceBookmark stored;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || validate_bookmark(bookmark) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, bookmark->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        status = reserve_items(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    stored = *bookmark;
    registry->revision = next_revision(registry->revision);
    stored.revision = registry->revision;
    registry->items[index] = stored;
    return UMI_STATUS_OK;
}

/*
 * Remove editor source bookmark registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_source_bookmark_registry_remove(
    UmiEditorSourceBookmarkRegistry *registry,
    const char *id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find editor source bookmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_source_bookmark_registry_find(
    const UmiEditorSourceBookmarkRegistry *registry,
    const char *id,
    UmiEditorSourceBookmark *out_bookmark)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor source bookmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_source_bookmark_registry_at(
    const UmiEditorSourceBookmarkRegistry *registry,
    size_t index,
    UmiEditorSourceBookmark *out_bookmark)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = registry->items[index];
    return UMI_STATUS_OK;
}

/* Provide the is candidate operation used by this module and its client applications. */
static int is_candidate(const UmiEditorSourceBookmark *bookmark,
                        const UmiEditorSourceLocation *from,
                        const char *group)
{
    return bookmark->enabled &&
           strcmp(bookmark->location.uri, from->uri) == 0 &&
           (group == NULL || group[0] == '\0' ||
            strcmp(bookmark->group, group) == 0);
}

/* Provide the traverse operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_bookmark == NULL ||
        umi_editor_source_location_validate(from) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiEditorSourceBookmark *candidate = &registry->items[index];
        int order;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!is_candidate(candidate, from, group)) continue;
        order = umi_editor_source_location_compare(&candidate->location, from);
        /* Apply this branch only when its contract condition is satisfied. */
        if (forward && order > 0 &&
            (best == NULL || umi_editor_source_location_compare(
                &candidate->location, &best->location) < 0)) {
            best = candidate;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!forward && order < 0 &&
                   (best == NULL || umi_editor_source_location_compare(
                       &candidate->location, &best->location) > 0)) {
            best = candidate;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (forward &&
            (wrapped == NULL || umi_editor_source_location_compare(
                &candidate->location, &wrapped->location) < 0)) {
            wrapped = candidate;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!forward &&
                   (wrapped == NULL || umi_editor_source_location_compare(
                       &candidate->location, &wrapped->location) > 0)) {
            wrapped = candidate;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL && wrap) best = wrapped;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_bookmark = *best;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor source bookmark registry next operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_next(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark)
{
    return traverse(registry, from, group, 1, wrap, out_bookmark);
}

/*
 * Provide the editor source bookmark registry previous operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_previous(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark)
{
    return traverse(registry, from, group, 0, wrap, out_bookmark);
}

/*
 * Return the number of records represented by editor source bookmark registry without
 * changing their state.
 */
size_t umi_editor_source_bookmark_registry_count(
    const UmiEditorSourceBookmarkRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the editor source bookmark registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_source_bookmark_registry_revision(
    const UmiEditorSourceBookmarkRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
