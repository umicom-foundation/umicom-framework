/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/recent_items.h
 *
 * PURPOSE:
 *   Define a cross-product most-recently-used resource catalogue.
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
#ifndef UMICOM_PLATFORM_RECENT_ITEMS_H
#define UMICOM_PLATFORM_RECENT_ITEMS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_RECENT_ITEMS_CAPACITY 1024U

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

typedef struct UmiRecentItemRegistry UmiRecentItemRegistry;

UmiStatus umi_platform_recent_items_registry_create(UmiRecentItemRegistry **out_registry);
void umi_platform_recent_items_registry_destroy(UmiRecentItemRegistry *registry);
UmiStatus umi_platform_recent_items_registry_upsert(UmiRecentItemRegistry *registry, const UmiRecentItemSnapshot *item);
UmiStatus umi_platform_recent_items_registry_remove(UmiRecentItemRegistry *registry, const char *id);
UmiStatus umi_platform_recent_items_registry_find(const UmiRecentItemRegistry *registry, const char *id, UmiRecentItemSnapshot *out_item);
UmiStatus umi_platform_recent_items_registry_at(const UmiRecentItemRegistry *registry, size_t index, UmiRecentItemSnapshot *out_item);
UmiStatus umi_platform_recent_items_registry_touch(UmiRecentItemRegistry *registry,
                                                     const char *id,
                                                     uint64_t opened_at);
UmiStatus umi_platform_recent_items_registry_set_pinned(UmiRecentItemRegistry *registry,
                                                        const char *id,
                                                        int pinned);
size_t umi_platform_recent_items_registry_count(const UmiRecentItemRegistry *registry);
uint64_t umi_platform_recent_items_registry_revision(const UmiRecentItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
