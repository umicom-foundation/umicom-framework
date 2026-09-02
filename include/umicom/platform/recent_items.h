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
 * Return the number of records represented by platform recent items registry without
 * changing their state.
 */
size_t umi_platform_recent_items_registry_count(const UmiRecentItemRegistry *registry);
/**
 * Provide the platform recent items registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_platform_recent_items_registry_revision(const UmiRecentItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
