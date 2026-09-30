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

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PRODUCT_MARKETPLACE_H
#define UMICOM_PRODUCT_MARKETPLACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_MARKETPLACE_CAPACITY 2048U

/**
 * Represent the product marketplace item snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the product marketplace item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProductMarketplaceItemRegistry UmiProductMarketplaceItemRegistry;

/**
 * Initialise product marketplace registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_product_marketplace_registry_create(UmiProductMarketplaceItemRegistry **out_registry);
/**
 * Release or reset state held by product marketplace registry so the same storage can be
 * reused safely.
 */
void umi_product_marketplace_registry_destroy(UmiProductMarketplaceItemRegistry *registry);
/**
 * Provide the product marketplace registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_marketplace_registry_upsert(UmiProductMarketplaceItemRegistry *registry, const UmiProductMarketplaceItemSnapshot *item);
/**
 * Remove product marketplace registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_product_marketplace_registry_remove(UmiProductMarketplaceItemRegistry *registry, const char *id);
/**
 * Find product marketplace registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_product_marketplace_registry_find(const UmiProductMarketplaceItemRegistry *registry, const char *id, UmiProductMarketplaceItemSnapshot *out_item);
/**
 * Find product marketplace registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_product_marketplace_registry_at(const UmiProductMarketplaceItemRegistry *registry, size_t index, UmiProductMarketplaceItemSnapshot *out_item);
/**
 * Return the number of records represented by product marketplace registry without
 * changing their state.
 */
size_t umi_product_marketplace_registry_count(const UmiProductMarketplaceItemRegistry *registry);
/**
 * Provide the product marketplace registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_product_marketplace_registry_revision(const UmiProductMarketplaceItemRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_product_marketplace_snapshot_validate(const UmiProductMarketplaceItemSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_PRODUCT_MARKETPLACE_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_product_marketplace_registry_upsert_many(UmiProductMarketplaceItemRegistry *registry,
    const UmiProductMarketplaceItemSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
