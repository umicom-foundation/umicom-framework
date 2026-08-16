/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_bookmark.h
 *
 * PURPOSE:
 *   Define source-aware bookmarks with groups, descriptions and deterministic
 *   next/previous traversal independently of any product or GUI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiEditorSourceBookmarkRegistry
    UmiEditorSourceBookmarkRegistry;

UmiStatus umi_editor_source_bookmark_registry_create(
    UmiEditorSourceBookmarkRegistry **out_registry);
void umi_editor_source_bookmark_registry_destroy(
    UmiEditorSourceBookmarkRegistry *registry);
UmiStatus umi_editor_source_bookmark_registry_upsert(
    UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceBookmark *bookmark);
UmiStatus umi_editor_source_bookmark_registry_remove(
    UmiEditorSourceBookmarkRegistry *registry,
    const char *id);
UmiStatus umi_editor_source_bookmark_registry_find(
    const UmiEditorSourceBookmarkRegistry *registry,
    const char *id,
    UmiEditorSourceBookmark *out_bookmark);
UmiStatus umi_editor_source_bookmark_registry_at(
    const UmiEditorSourceBookmarkRegistry *registry,
    size_t index,
    UmiEditorSourceBookmark *out_bookmark);
UmiStatus umi_editor_source_bookmark_registry_next(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark);
UmiStatus umi_editor_source_bookmark_registry_previous(
    const UmiEditorSourceBookmarkRegistry *registry,
    const UmiEditorSourceLocation *from,
    const char *group,
    int wrap,
    UmiEditorSourceBookmark *out_bookmark);
size_t umi_editor_source_bookmark_registry_count(
    const UmiEditorSourceBookmarkRegistry *registry);
uint64_t umi_editor_source_bookmark_registry_revision(
    const UmiEditorSourceBookmarkRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_BOOKMARK_H */
