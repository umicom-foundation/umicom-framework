/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/bookmarks.h
 *
 * PURPOSE:
 *   Define reusable resource bookmarks for file, workspace and remote locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the bookmark snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the bookmark registry data shared with callers of this public contract.
 */
typedef struct UmiBookmarkRegistry UmiBookmarkRegistry;

/**
 * Initialise platform bookmarks registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_platform_bookmarks_registry_create(UmiBookmarkRegistry **out_registry);
/**
 * Release or reset state held by platform bookmarks registry so the same storage can be
 * reused safely.
 */
void umi_platform_bookmarks_registry_destroy(UmiBookmarkRegistry *registry);
/**
 * Provide the platform bookmarks registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_bookmarks_registry_upsert(UmiBookmarkRegistry *registry, const UmiBookmarkSnapshot *item);
/**
 * Remove platform bookmarks registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_platform_bookmarks_registry_remove(UmiBookmarkRegistry *registry, const char *id);
/**
 * Find platform bookmarks registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_platform_bookmarks_registry_find(const UmiBookmarkRegistry *registry, const char *id, UmiBookmarkSnapshot *out_item);
/**
 * Find platform bookmarks registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_platform_bookmarks_registry_at(const UmiBookmarkRegistry *registry, size_t index, UmiBookmarkSnapshot *out_item);
/**
 * Return the number of records represented by platform bookmarks registry without changing
 * their state.
 */
size_t umi_platform_bookmarks_registry_count(const UmiBookmarkRegistry *registry);
/**
 * Provide the platform bookmarks registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_platform_bookmarks_registry_revision(const UmiBookmarkRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
