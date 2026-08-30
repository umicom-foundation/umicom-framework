/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/metadata_provider.h
 *
 * PURPOSE:
 *   Define metadata providers that can feed the reusable Umicom product catalogue.
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
#ifndef UMICOM_PRODUCT_METADATA_PROVIDER_H
#define UMICOM_PRODUCT_METADATA_PROVIDER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_METADATA_PROVIDER_CAPACITY 128U

typedef struct UmiProductMetadataProviderSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char endpoint[512];
    char kind[64];
    int enabled;
    int trusted;
    int priority;
    uint64_t revision;
} UmiProductMetadataProviderSnapshot;

typedef struct UmiProductMetadataProviderRegistry UmiProductMetadataProviderRegistry;

UmiStatus umi_product_metadata_provider_registry_create(UmiProductMetadataProviderRegistry **out_registry);
void umi_product_metadata_provider_registry_destroy(UmiProductMetadataProviderRegistry *registry);
UmiStatus umi_product_metadata_provider_registry_upsert(UmiProductMetadataProviderRegistry *registry, const UmiProductMetadataProviderSnapshot *item);
UmiStatus umi_product_metadata_provider_registry_remove(UmiProductMetadataProviderRegistry *registry, const char *id);
UmiStatus umi_product_metadata_provider_registry_find(const UmiProductMetadataProviderRegistry *registry, const char *id, UmiProductMetadataProviderSnapshot *out_item);
UmiStatus umi_product_metadata_provider_registry_at(const UmiProductMetadataProviderRegistry *registry, size_t index, UmiProductMetadataProviderSnapshot *out_item);
size_t umi_product_metadata_provider_registry_count(const UmiProductMetadataProviderRegistry *registry);
uint64_t umi_product_metadata_provider_registry_revision(const UmiProductMetadataProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
