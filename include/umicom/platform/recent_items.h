/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/recent_items.h
 *
 * PURPOSE:
 *   Define a cross-product most-recently-used resource catalogue.
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
#ifndef UMICOM_PLATFORM_RECENT_ITEMS_H
#define UMICOM_PLATFORM_RECENT_ITEMS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_RECENT_ITEMS_CAPACITY 1024U

/**
 * Represent the recent item snapshot data shared with callers of this public contract.
 */
typedef struct UmiRecentItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char label[256];
    char kind[64];
    uint64_t last_opened;
    uint64_t open_count;
    int pinned;
    uint64_t revision;
} UmiRecentItemSnapshot;

/**
 * Represent the recent item registry data shared with callers of this public contract.
 */
typedef struct UmiRecentItemRegistry UmiRecentItemRegistry;

/**
 * Describe a bounded recent-item search. Empty text and kind values mean
 * "match every value". Set pinned_only to a non-zero value when a surface
 * should show only items that the user deliberately kept.
 */
typedef struct UmiRecentItemQuery {
    const char *text;
    const char *kind;
    int pinned_only;
    size_t limit;
} UmiRecentItemQuery;

/**
 * Build a stable bounded identifier from a resource URI. Applications supply
 * a short scope such as "studio-workspace" to keep different item families
 * distinct without duplicating hashing code.
 */
UmiStatus umi_platform_recent_item_id_from_uri(
    const char *scope,
    const char *uri,
    char *out_id,
    size_t out_capacity);

/**
 * Initialise platform recent items registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_recent_items_registry_create(UmiRecentItemRegistry **out_registry);
/**
 * Release or reset state held by platform recent items registry so the same storage can be
 * reused safely.
 */
void umi_platform_recent_items_registry_destroy(UmiRecentItemRegistry *registry);
/**
 * Provide the platform recent items registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_recent_items_registry_upsert(UmiRecentItemRegistry *registry, const UmiRecentItemSnapshot *item);
/**
 * Remove platform recent items registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_platform_recent_items_registry_remove(UmiRecentItemRegistry *registry, const char *id);
/**
 * Find platform recent items registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_recent_items_registry_find(const UmiRecentItemRegistry *registry, const char *id, UmiRecentItemSnapshot *out_item);
/**
 * Find platform recent items registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_recent_items_registry_at(const UmiRecentItemRegistry *registry, size_t index, UmiRecentItemSnapshot *out_item);
/**
 * Provide the platform recent items registry touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_recent_items_registry_touch(UmiRecentItemRegistry *registry,
                                                     const char *id,
                                                     uint64_t opened_at);
/**
 * Provide the platform recent items registry set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_recent_items_registry_set_pinned(UmiRecentItemRegistry *registry,
                                                        const char *id,
                                                        int pinned);
/**
 * Remove the oldest unpinned items until the registry reaches maximum_count.
 * Pinned work is preserved even when pinned items alone exceed that limit.
 */
UmiStatus umi_platform_recent_items_registry_trim(
    UmiRecentItemRegistry *registry,
    size_t maximum_count);
/**
 * Return the number of records represented by platform recent items registry without
 * changing their state.
 */
size_t umi_platform_recent_items_registry_count(const UmiRecentItemRegistry *registry);
/**
 * Provide the platform recent items registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_platform_recent_items_registry_revision(const UmiRecentItemRegistry *registry);

/**
 * Copy the best matching recent items into caller-owned storage. Results are
 * ordered with pinned items first, followed by the most recently opened item.
 * A zero query limit uses the full output capacity.
 */
UmiStatus umi_platform_recent_items_registry_query(
    const UmiRecentItemRegistry *registry,
    const UmiRecentItemQuery *query,
    UmiRecentItemSnapshot *out_items,
    size_t out_capacity,
    size_t *out_count);

/**
 * Save the complete registry with atomic file replacement. Text fields are
 * encoded so paths and labels cannot break record boundaries.
 */
UmiStatus umi_platform_recent_items_registry_save(
    const UmiRecentItemRegistry *registry,
    const char *path);

/**
 * Load a registry transactionally. A missing file returns an empty registry
 * and sets out_loaded to zero; malformed content never leaks a partial model.
 */
UmiStatus umi_platform_recent_items_registry_load(
    const char *path,
    UmiRecentItemRegistry **out_registry,
    int *out_loaded);

#ifdef __cplusplus
}
#endif

#endif
