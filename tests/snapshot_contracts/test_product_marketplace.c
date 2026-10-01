/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_product_marketplace.c
 * PURPOSE: Check product marketplace input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/product/marketplace.h"
#include "umicom/product/marketplace.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProductMarketplaceItemSnapshot
#define CONTRACT_REGISTRY UmiProductMarketplaceItemRegistry
#define CONTRACT_CAPACITY UMI_PRODUCT_MARKETPLACE_CAPACITY
#define CONTRACT_VALIDATE umi_product_marketplace_snapshot_validate
#define CONTRACT_BATCH umi_product_marketplace_registry_upsert_many
#define CONTRACT_CREATE umi_product_marketplace_registry_create
#define CONTRACT_DESTROY umi_product_marketplace_registry_destroy
#define CONTRACT_UPSERT umi_product_marketplace_registry_upsert
#define CONTRACT_REMOVE umi_product_marketplace_registry_remove
#define CONTRACT_FIND umi_product_marketplace_registry_find
#define CONTRACT_AT umi_product_marketplace_registry_at
#define CONTRACT_COUNT umi_product_marketplace_registry_count
#define CONTRACT_REVISION umi_product_marketplace_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProductMarketplaceItemSnapshot, id), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->id), 1 },
    {"provider_id", offsetof(UmiProductMarketplaceItemSnapshot, provider_id), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->provider_id), 0 },
    {"name", offsetof(UmiProductMarketplaceItemSnapshot, name), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->name), 0 },
    {"summary", offsetof(UmiProductMarketplaceItemSnapshot, summary), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->summary), 0 },
    {"version", offsetof(UmiProductMarketplaceItemSnapshot, version), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->version), 0 },
    {"category", offsetof(UmiProductMarketplaceItemSnapshot, category), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->category), 0 },
    {"licence", offsetof(UmiProductMarketplaceItemSnapshot, licence), sizeof(((UmiProductMarketplaceItemSnapshot *)0)->licence), 0 }
};
static int ContractSnapshotEqual(const UmiProductMarketplaceItemSnapshot *left, const UmiProductMarketplaceItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->provider_id, right->provider_id, sizeof(left->provider_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->summary, right->summary, sizeof(left->summary)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->licence, right->licence, sizeof(left->licence)) == 0 &&
        left->installed == right->installed &&
        left->update_available == right->update_available &&
        left->trusted == right->trusted &&
        left->compatible == right->compatible &&
        left->rank == right->rank &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiProductMarketplaceItemSnapshot *item)
{
    item->provider_id[0] = 'v';
    item->name[0] = 'v';
    item->summary[0] = 'v';
    item->version[0] = 'v';
    item->category[0] = 'v';
    item->licence[0] = 'v';
    item->installed = (int)10U;
    item->update_available = (int)11U;
    item->trusted = (int)12U;
    item->compatible = (int)13U;
    item->rank = (int32_t)14U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_product_marketplace_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_product_marketplace_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProductMarketplaceItemEdit
#define CONTRACT_EDIT_CURRENT umi_product_marketplace_registry_edit_if_current
#include "snapshot_contract_cases.h"
