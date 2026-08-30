/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/marketplace.h
 *
 * PURPOSE:
 *   Define a provider-neutral marketplace catalogue for Umicom applications, plug-ins and templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PRODUCT_MARKETPLACE_H
#define UMICOM_PRODUCT_MARKETPLACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_MARKETPLACE_CAPACITY 2048U

typedef struct UmiProductMarketplaceItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char provider_id[128];
    char name[256];
    char summary[512];
    char version[64];
    char category[128];
    char licence[128];
    int installed;
    int update_available;
    int trusted;
    int compatible;
    int32_t rank;
    uint64_t revision;
} UmiProductMarketplaceItemSnapshot;

typedef struct UmiProductMarketplaceItemRegistry UmiProductMarketplaceItemRegistry;

UmiStatus umi_product_marketplace_registry_create(UmiProductMarketplaceItemRegistry **out_registry);
void umi_product_marketplace_registry_destroy(UmiProductMarketplaceItemRegistry *registry);
UmiStatus umi_product_marketplace_registry_upsert(UmiProductMarketplaceItemRegistry *registry, const UmiProductMarketplaceItemSnapshot *item);
UmiStatus umi_product_marketplace_registry_remove(UmiProductMarketplaceItemRegistry *registry, const char *id);
UmiStatus umi_product_marketplace_registry_find(const UmiProductMarketplaceItemRegistry *registry, const char *id, UmiProductMarketplaceItemSnapshot *out_item);
UmiStatus umi_product_marketplace_registry_at(const UmiProductMarketplaceItemRegistry *registry, size_t index, UmiProductMarketplaceItemSnapshot *out_item);
size_t umi_product_marketplace_registry_count(const UmiProductMarketplaceItemRegistry *registry);
uint64_t umi_product_marketplace_registry_revision(const UmiProductMarketplaceItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
