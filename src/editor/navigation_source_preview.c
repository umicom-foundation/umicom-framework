/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_source_preview.c
 *
 * PURPOSE:
 *   Implement a bounded source-preview cache above provider adapters.  The
 *   cache is revision-aware, never opens files itself, and uses deterministic
 *   oldest-entry eviction so local, remote and virtual documents obey the same
 *   frontend-independent contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_source_preview.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationSourcePreviewCache {
    UmiEditorNavigationProviderRegistry *registry;
    UmiEditorNavigationSourcePreview *items;
    size_t count;
    size_t capacity;
    size_t maximum_total_bytes;
    size_t total_bytes;
    uint64_t hit_count;
    uint64_t miss_count;
    uint64_t eviction_count;
    uint64_t sequence;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int cancelled_never(void *user_data)
{
    (void)user_data;
    return 0;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_request(
    const UmiEditorNavigationPreviewRequest *request)
{
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U ||
        !terminated(request->uri, sizeof(request->uri)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        request->uri[0] == '\0' || request->end_line < request->start_line ||
        request->maximum_bytes == 0U ||
        request->maximum_bytes >= UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_preview(
    const UmiEditorNavigationSourcePreview *preview,
    const UmiEditorNavigationPreviewRequest *request)
{
    if (preview == NULL ||
        preview->struct_size != (uint32_t)sizeof(*preview) ||
        preview->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        !terminated(preview->provider_id, sizeof(preview->provider_id)) ||
        !terminated(preview->uri, sizeof(preview->uri)) ||
        !terminated(preview->language_id, sizeof(preview->language_id)) ||
        !terminated(preview->content, sizeof(preview->content)) ||
        preview->uri[0] == '\0' || preview->provider_id[0] == '\0' ||
        preview->end_line < preview->start_line ||
        preview->content_length != strlen(preview->content) ||
        preview->content_length > request->maximum_bytes ||
        strcmp(preview->uri, request->uri) != 0 ||
        preview->start_line != request->start_line ||
        preview->end_line != request->end_line ||
        (request->language_id[0] != '\0' &&
         strcmp(preview->language_id, request->language_id) != 0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static int same_key(const UmiEditorNavigationSourcePreview *preview,
                    const UmiEditorNavigationPreviewRequest *request)
{
    return strcmp(preview->uri, request->uri) == 0 &&
           (request->language_id[0] == '\0' ||
            strcmp(preview->language_id, request->language_id) == 0) &&
           preview->start_line == request->start_line &&
           preview->end_line == request->end_line &&
           preview->content_length <= request->maximum_bytes &&
           (request->document_revision == 0U ||
            preview->document_revision == request->document_revision);
}

static size_t find_key(
    const UmiEditorNavigationSourcePreviewCache *cache,
    const UmiEditorNavigationPreviewRequest *request)
{
    size_t index;
    for (index = 0U; index < cache->count; ++index) {
        if (same_key(&cache->items[index], request)) return index;
    }
    return SIZE_MAX;
}

static size_t oldest_index(
    const UmiEditorNavigationSourcePreviewCache *cache)
{
    size_t index;
    size_t oldest = 0U;
    for (index = 1U; index < cache->count; ++index) {
        if (cache->items[index].sequence < cache->items[oldest].sequence) {
            oldest = index;
        }
    }
    return oldest;
}

static void remove_at(UmiEditorNavigationSourcePreviewCache *cache,
                      size_t index)
{
    if (index >= cache->count) return;
    if (cache->total_bytes >= cache->items[index].content_length) {
        cache->total_bytes -= cache->items[index].content_length;
    } else {
        cache->total_bytes = 0U;
    }
    if (index + 1U < cache->count) {
        (void)memmove(&cache->items[index], &cache->items[index + 1U],
                      (cache->count - index - 1U) * sizeof(*cache->items));
    }
    --cache->count;
}

static UmiStatus store_preview(
    UmiEditorNavigationSourcePreviewCache *cache,
    UmiEditorNavigationSourcePreview *preview)
{
    UmiEditorNavigationPreviewRequest key;
    size_t index;

    if (preview->content_length > cache->maximum_total_bytes) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&key, 0, sizeof(key));
    key.uri[0] = '\0';
    (void)memcpy(key.uri, preview->uri, strlen(preview->uri) + 1U);
    (void)memcpy(key.language_id, preview->language_id,
                 strlen(preview->language_id) + 1U);
    key.maximum_bytes = UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY - 1U;
    key.start_line = preview->start_line;
    key.end_line = preview->end_line;
    key.document_revision = preview->document_revision;
    index = find_key(cache, &key);
    if (index != SIZE_MAX) remove_at(cache, index);
    while (cache->count >= cache->capacity ||
           cache->total_bytes + preview->content_length >
               cache->maximum_total_bytes) {
        if (cache->count == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
        remove_at(cache, oldest_index(cache));
        ++cache->eviction_count;
    }
    preview->sequence = ++cache->sequence;
    cache->items[cache->count++] = *preview;
    cache->total_bytes += preview->content_length;
    cache->revision = next_revision(cache->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_source_preview_config_initialize(
    UmiEditorNavigationSourcePreviewConfig *config)
{
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(config, 0, sizeof(*config));
    config->struct_size = (uint32_t)sizeof(*config);
    config->api_version = UMI_EDITOR_NAVIGATION_SOURCE_PREVIEW_API_VERSION;
    config->capacity = UMI_EDITOR_NAVIGATION_PREVIEW_DEFAULT_CAPACITY;
    config->maximum_total_bytes =
        UMI_EDITOR_NAVIGATION_PREVIEW_DEFAULT_MAXIMUM_BYTES;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_source_preview_cache_create(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationSourcePreviewConfig *config,
    UmiEditorNavigationSourcePreviewCache **out_cache)
{
    UmiEditorNavigationSourcePreviewConfig effective;
    UmiEditorNavigationSourcePreviewCache *cache;

    if (registry == NULL || out_cache == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_cache = NULL;
    if (config == NULL) {
        (void)umi_editor_navigation_source_preview_config_initialize(&effective);
    } else {
        effective = *config;
    }
    if (effective.struct_size != (uint32_t)sizeof(effective) ||
        effective.api_version !=
            UMI_EDITOR_NAVIGATION_SOURCE_PREVIEW_API_VERSION ||
        effective.capacity == 0U ||
        effective.capacity > UMI_EDITOR_NAVIGATION_PREVIEW_MAXIMUM_CAPACITY ||
        effective.maximum_total_bytes == 0U ||
        effective.maximum_total_bytes >
            UMI_EDITOR_NAVIGATION_PREVIEW_MAXIMUM_BYTES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cache = (UmiEditorNavigationSourcePreviewCache *)calloc(1U,
                                                             sizeof(*cache));
    if (cache == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    cache->items = (UmiEditorNavigationSourcePreview *)calloc(
        effective.capacity, sizeof(*cache->items));
    if (cache->items == NULL) {
        free(cache);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    cache->registry = registry;
    cache->capacity = effective.capacity;
    cache->maximum_total_bytes = effective.maximum_total_bytes;
    cache->revision = 1U;
    *out_cache = cache;
    return UMI_STATUS_OK;
}

void umi_editor_navigation_source_preview_cache_destroy(
    UmiEditorNavigationSourcePreviewCache *cache)
{
    if (cache == NULL) return;
    free(cache->items);
    cache->items = NULL;
    free(cache);
}

UmiStatus umi_editor_navigation_source_preview_cache_clear(
    UmiEditorNavigationSourcePreviewCache *cache)
{
    if (cache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(cache->items, 0, cache->capacity * sizeof(*cache->items));
    cache->count = 0U;
    cache->total_bytes = 0U;
    cache->revision = next_revision(cache->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_source_preview_cache_resolve(
    UmiEditorNavigationSourcePreviewCache *cache,
    const UmiEditorNavigationPreviewRequest *request,
    UmiEditorNavigationSourcePreview *out_preview)
{
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorNavigationCancellation cancellation;
    size_t index;
    UmiStatus first_failure = UMI_STATUS_NOT_FOUND;

    if (cache == NULL || out_preview == NULL ||
        validate_request(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_key(cache, request);
    if (index != SIZE_MAX) {
        cache->items[index].sequence = ++cache->sequence;
        *out_preview = cache->items[index];
        out_preview->stale = out_preview->stale ||
            (request->document_revision != 0U &&
             out_preview->document_revision != 0U &&
             request->document_revision != out_preview->document_revision);
        ++cache->hit_count;
        cache->revision = next_revision(cache->revision);
        return UMI_STATUS_OK;
    }
    ++cache->miss_count;
    cancellation.struct_size = (uint32_t)sizeof(cancellation);
    cancellation.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    cancellation.is_cancelled = cancelled_never;
    cancellation.user_data = NULL;
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(cache->registry);
         ++index) {
        UmiEditorNavigationSourcePreview preview;
        UmiStatus status;
        if (umi_editor_navigation_provider_registry_at(cache->registry, index,
                                                       &registration) !=
                UMI_STATUS_OK ||
            !umi_editor_navigation_provider_supports_preview(&registration,
                                                             request)) {
            continue;
        }
        (void)memset(&preview, 0, sizeof(preview));
        status = registration.functions.preview(
            registration.instance, request, &cancellation, &preview);
        if (status == UMI_STATUS_NOT_FOUND) continue;
        if (status != UMI_STATUS_OK) {
            if (first_failure == UMI_STATUS_NOT_FOUND) first_failure = status;
            continue;
        }
        {
            size_t length = strlen(registration.descriptor.id);
            if (length >= sizeof(preview.provider_id)) {
                first_failure = UMI_STATUS_CAPACITY_EXCEEDED;
                continue;
            }
            (void)memcpy(preview.provider_id, registration.descriptor.id,
                         length + 1U);
        }
        if (validate_preview(&preview, request) != UMI_STATUS_OK) {
            first_failure = UMI_STATUS_INVALID_STATE;
            continue;
        }
        {
            int stale = preview.stale ||
                (request->document_revision != 0U &&
                 preview.document_revision != 0U &&
                 request->document_revision != preview.document_revision);
            preview.stale = preview.stale != 0;
            status = store_preview(cache, &preview);
            if (status != UMI_STATUS_OK) return status;
            *out_preview = preview;
            out_preview->stale = stale;
        }
        return UMI_STATUS_OK;
    }
    return first_failure;
}

UmiStatus umi_editor_navigation_source_preview_cache_invalidate_uri(
    UmiEditorNavigationSourcePreviewCache *cache,
    const char *uri,
    size_t *out_removed_count)
{
    size_t index = 0U;
    size_t removed = 0U;

    if (out_removed_count != NULL) *out_removed_count = 0U;
    if (cache == NULL || uri == NULL || uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    while (index < cache->count) {
        if (strcmp(cache->items[index].uri, uri) == 0) {
            remove_at(cache, index);
            ++removed;
        } else {
            ++index;
        }
    }
    if (removed == 0U) return UMI_STATUS_NOT_FOUND;
    cache->revision = next_revision(cache->revision);
    if (out_removed_count != NULL) *out_removed_count = removed;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_source_preview_cache_at(
    const UmiEditorNavigationSourcePreviewCache *cache,
    size_t position,
    UmiEditorNavigationSourcePreview *out_preview)
{
    if (cache == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= cache->count) return UMI_STATUS_NOT_FOUND;
    *out_preview = cache->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_source_preview_cache_snapshot(
    const UmiEditorNavigationSourcePreviewCache *cache,
    UmiEditorNavigationSourcePreviewCacheSnapshot *out_snapshot)
{
    if (cache == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_NAVIGATION_SOURCE_PREVIEW_API_VERSION;
    out_snapshot->entry_count = cache->count;
    out_snapshot->total_bytes = cache->total_bytes;
    out_snapshot->capacity = cache->capacity;
    out_snapshot->maximum_total_bytes = cache->maximum_total_bytes;
    out_snapshot->hit_count = cache->hit_count;
    out_snapshot->miss_count = cache->miss_count;
    out_snapshot->eviction_count = cache->eviction_count;
    out_snapshot->revision = cache->revision;
    return UMI_STATUS_OK;
}

size_t umi_editor_navigation_source_preview_cache_count(
    const UmiEditorNavigationSourcePreviewCache *cache)
{
    return cache != NULL ? cache->count : 0U;
}

uint64_t umi_editor_navigation_source_preview_cache_revision(
    const UmiEditorNavigationSourcePreviewCache *cache)
{
    return cache != NULL ? cache->revision : 0U;
}
