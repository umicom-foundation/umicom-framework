/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_bookmark.h
 *
 * PURPOSE:
 *   Define source-aware bookmarks with groups, descriptions and deterministic
 *   next/previous traversal independently of any product or GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SOURCE_BOOKMARK_H
#define UMICOM_EDITOR_SOURCE_BOOKMARK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SOURCE_BOOKMARK_API_VERSION 1U
#define UMI_EDITOR_SOURCE_BOOKMARK_ID_CAPACITY 128U
#define UMI_EDITOR_SOURCE_BOOKMARK_GROUP_CAPACITY 128U
#define UMI_EDITOR_SOURCE_BOOKMARK_DESCRIPTION_CAPACITY 512U

/**
 * Represent the editor source bookmark data shared with callers of this public contract.
 */
typedef struct UmiEditorSourceBookmark {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SOURCE_BOOKMARK_ID_CAPACITY];
    char group[UMI_EDITOR_SOURCE_BOOKMARK_GROUP_CAPACITY];
    char description[UMI_EDITOR_SOURCE_BOOKMARK_DESCRIPTION_CAPACITY];
    UmiEditorSourceLocation location;
    uint64_t revision;
    int enabled;
    int persistent;
} UmiEditorSourceBookmark;

/**
 * Represent the editor source bookmark registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSourceBookmarkRegistry
    UmiEditorSourceBookmarkRegistry;

/**
 * Initialise editor source bookmark registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_source_bookmark_registry_create(
    UmiEditorSourceBookmarkRegistry **out_registry);
/**
 * Release or reset state held by editor source bookmark registry so the same storage can
 * be reused safely.
 */
void umi_editor_source_bookmark_registry_destroy(
    UmiEditorSourceBookmarkRegistry *registry);
/**
 * Provide the editor source bookmark registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_upsert(
    UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceBookmark *bookmark);
/**
 * Remove editor source bookmark registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_source_bookmark_registry_remove(
    UmiEditorSourceBookmarkRegistry *registry,
    const char *id);
/**
 * Find editor source bookmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_source_bookmark_registry_find(
    const UmiEditorSourceBookmarkRegistry *registry,
    const char *id,
    UmiEditorSourceBookmark *out_bookmark);
/**
 * Find editor source bookmark registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_source_bookmark_registry_at(
    const UmiEditorSourceBookmarkRegistry *registry,
    size_t index,
    UmiEditorSourceBookmark *out_bookmark);
/**
 * Provide the editor source bookmark registry next operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_next(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark);
/**
 * Provide the editor source bookmark registry previous operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_source_bookmark_registry_previous(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark);
/**
 * Return the number of records represented by editor source bookmark registry without
 * changing their state.
 */
size_t umi_editor_source_bookmark_registry_count(
    const UmiEditorSourceBookmarkRegistry *registry);
/**
 * Provide the editor source bookmark registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_source_bookmark_registry_revision(
    const UmiEditorSourceBookmarkRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_BOOKMARK_H */
