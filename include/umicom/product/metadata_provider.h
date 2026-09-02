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

/*
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

/**
 * Represent the product metadata provider snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the product metadata provider registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProductMetadataProviderRegistry UmiProductMetadataProviderRegistry;

/**
 * Initialise product metadata provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_product_metadata_provider_registry_create(UmiProductMetadataProviderRegistry **out_registry);
/**
 * Release or reset state held by product metadata provider registry so the same storage
 * can be reused safely.
 */
void umi_product_metadata_provider_registry_destroy(UmiProductMetadataProviderRegistry *registry);
/**
 * Provide the product metadata provider registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_product_metadata_provider_registry_upsert(UmiProductMetadataProviderRegistry *registry, const UmiProductMetadataProviderSnapshot *item);
/**
 * Remove product metadata provider registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_product_metadata_provider_registry_remove(UmiProductMetadataProviderRegistry *registry, const char *id);
/**
 * Find product metadata provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_metadata_provider_registry_find(const UmiProductMetadataProviderRegistry *registry, const char *id, UmiProductMetadataProviderSnapshot *out_item);
/**
 * Find product metadata provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_metadata_provider_registry_at(const UmiProductMetadataProviderRegistry *registry, size_t index, UmiProductMetadataProviderSnapshot *out_item);
/**
 * Return the number of records represented by product metadata provider registry without
 * changing their state.
 */
size_t umi_product_metadata_provider_registry_count(const UmiProductMetadataProviderRegistry *registry);
/**
 * Provide the product metadata provider registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_product_metadata_provider_registry_revision(const UmiProductMetadataProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
