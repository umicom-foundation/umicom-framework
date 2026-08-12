/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/bookmarks.h
 *
 * PURPOSE:
 *   Define reusable resource bookmarks for file, workspace and remote locations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_BOOKMARKS_H
#define UMICOM_PLATFORM_BOOKMARKS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_BOOKMARKS_CAPACITY 512U

typedef struct UmiBookmarkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char label[256];
    char group[128];
    char icon_name[128];
    int32_t order;
    uint64_t revision;
} UmiBookmarkSnapshot;

typedef struct UmiBookmarkRegistry UmiBookmarkRegistry;

UmiStatus umi_platform_bookmarks_registry_create(UmiBookmarkRegistry **out_registry);
void umi_platform_bookmarks_registry_destroy(UmiBookmarkRegistry *registry);
UmiStatus umi_platform_bookmarks_registry_upsert(UmiBookmarkRegistry *registry, const UmiBookmarkSnapshot *item);
UmiStatus umi_platform_bookmarks_registry_remove(UmiBookmarkRegistry *registry, const char *id);
UmiStatus umi_platform_bookmarks_registry_find(const UmiBookmarkRegistry *registry, const char *id, UmiBookmarkSnapshot *out_item);
UmiStatus umi_platform_bookmarks_registry_at(const UmiBookmarkRegistry *registry, size_t index, UmiBookmarkSnapshot *out_item);
size_t umi_platform_bookmarks_registry_count(const UmiBookmarkRegistry *registry);
uint64_t umi_platform_bookmarks_registry_revision(const UmiBookmarkRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
