/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_source_preview.h
 *
 * PURPOSE:
 *   Resolve and cache bounded, revision-aware source excerpts for peek,
 *   references and hierarchy surfaces without granting application code direct
 *   access to local, remote or virtual-document storage implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the editor navigation source preview config data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorNavigationSourcePreviewConfig {
    uint32_t struct_size;
    uint32_t api_version;
    size_t capacity;
    size_t maximum_total_bytes;
} UmiEditorNavigationSourcePreviewConfig;

/**
 * Represent the editor navigation source preview cache snapshot data shared with callers
 * of this public contract.
 */
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

/**
 * Represent the editor navigation source preview cache data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorNavigationSourcePreviewCache
    UmiEditorNavigationSourcePreviewCache;

/**
 * Initialise editor navigation source preview config from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_source_preview_config_initialize(
    UmiEditorNavigationSourcePreviewConfig *config);
/**
 * Initialise editor navigation source preview cache from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_source_preview_cache_create(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationSourcePreviewConfig *config,
    UmiEditorNavigationSourcePreviewCache **out_cache);
/**
 * Release or reset state held by editor navigation source preview cache so the same
 * storage can be reused safely.
 */
void umi_editor_navigation_source_preview_cache_destroy(
    UmiEditorNavigationSourcePreviewCache *cache);
/**
 * Release or reset state held by editor navigation source preview cache so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_navigation_source_preview_cache_clear(
    UmiEditorNavigationSourcePreviewCache *cache);
/**
 * Provide the editor navigation source preview cache resolve operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_source_preview_cache_resolve(
    UmiEditorNavigationSourcePreviewCache *cache,
    const UmiEditorNavigationPreviewRequest *request,
    UmiEditorNavigationSourcePreview *out_preview);
/**
 * Provide the editor navigation source preview cache invalidate uri operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_source_preview_cache_invalidate_uri(
    UmiEditorNavigationSourcePreviewCache *cache,
    const char *uri,
    size_t *out_removed_count);
/**
 * Find editor navigation source preview cache while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_navigation_source_preview_cache_at(
    const UmiEditorNavigationSourcePreviewCache *cache,
    size_t position,
    UmiEditorNavigationSourcePreview *out_preview);
/**
 * Provide the editor navigation source preview cache snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_source_preview_cache_snapshot(
    const UmiEditorNavigationSourcePreviewCache *cache,
    UmiEditorNavigationSourcePreviewCacheSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor navigation source preview cache
 * without changing their state.
 */
size_t umi_editor_navigation_source_preview_cache_count(
    const UmiEditorNavigationSourcePreviewCache *cache);
/**
 * Provide the editor navigation source preview cache revision operation used by this
 * module and its client applications.
 */
uint64_t umi_editor_navigation_source_preview_cache_revision(
    const UmiEditorNavigationSourcePreviewCache *cache);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_SOURCE_PREVIEW_H */
