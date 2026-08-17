/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_source_preview.h
 *
 * PURPOSE:
 *   Resolve and cache bounded, revision-aware source excerpts for peek,
 *   references and hierarchy surfaces without granting application code direct
 *   access to local, remote or virtual-document storage implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_SOURCE_PREVIEW_H
#define UMICOM_EDITOR_NAVIGATION_SOURCE_PREVIEW_H

#include "umicom/editor/navigation_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_SOURCE_PREVIEW_API_VERSION 1U
#define UMI_EDITOR_NAVIGATION_PREVIEW_DEFAULT_CAPACITY 64U
#define UMI_EDITOR_NAVIGATION_PREVIEW_MAXIMUM_CAPACITY 1024U
#define UMI_EDITOR_NAVIGATION_PREVIEW_DEFAULT_MAXIMUM_BYTES (512U * 1024U)
#define UMI_EDITOR_NAVIGATION_PREVIEW_MAXIMUM_BYTES (32U * 1024U * 1024U)

typedef struct UmiEditorNavigationSourcePreviewConfig {
    uint32_t struct_size;
    uint32_t api_version;
    size_t capacity;
    size_t maximum_total_bytes;
} UmiEditorNavigationSourcePreviewConfig;

typedef struct UmiEditorNavigationSourcePreviewCacheSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t entry_count;
    size_t total_bytes;
    size_t capacity;
    size_t maximum_total_bytes;
    uint64_t hit_count;
    uint64_t miss_count;
    uint64_t eviction_count;
    uint64_t revision;
} UmiEditorNavigationSourcePreviewCacheSnapshot;

typedef struct UmiEditorNavigationSourcePreviewCache
    UmiEditorNavigationSourcePreviewCache;

UmiStatus umi_editor_navigation_source_preview_config_initialize(
    UmiEditorNavigationSourcePreviewConfig *config);
UmiStatus umi_editor_navigation_source_preview_cache_create(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationSourcePreviewConfig *config,
    UmiEditorNavigationSourcePreviewCache **out_cache);
void umi_editor_navigation_source_preview_cache_destroy(
    UmiEditorNavigationSourcePreviewCache *cache);
UmiStatus umi_editor_navigation_source_preview_cache_clear(
    UmiEditorNavigationSourcePreviewCache *cache);
UmiStatus umi_editor_navigation_source_preview_cache_resolve(
    UmiEditorNavigationSourcePreviewCache *cache,
    const UmiEditorNavigationPreviewRequest *request,
    UmiEditorNavigationSourcePreview *out_preview);
UmiStatus umi_editor_navigation_source_preview_cache_invalidate_uri(
    UmiEditorNavigationSourcePreviewCache *cache,
    const char *uri,
    size_t *out_removed_count);
UmiStatus umi_editor_navigation_source_preview_cache_at(
    const UmiEditorNavigationSourcePreviewCache *cache,
    size_t position,
    UmiEditorNavigationSourcePreview *out_preview);
UmiStatus umi_editor_navigation_source_preview_cache_snapshot(
    const UmiEditorNavigationSourcePreviewCache *cache,
    UmiEditorNavigationSourcePreviewCacheSnapshot *out_snapshot);
size_t umi_editor_navigation_source_preview_cache_count(
    const UmiEditorNavigationSourcePreviewCache *cache);
uint64_t umi_editor_navigation_source_preview_cache_revision(
    const UmiEditorNavigationSourcePreviewCache *cache);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_SOURCE_PREVIEW_H */
