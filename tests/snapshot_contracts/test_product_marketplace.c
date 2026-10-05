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
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_product_marketplace_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProductMarketplaceItemSnapshot ArchiveSample(void)
{
    UmiProductMarketplaceItemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProductMarketplaceItemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->summary) + 1U;
        memset(value->summary + used, 0xa5, sizeof(value->summary) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->licence) + 1U;
        memset(value->licence + used, 0xa5, sizeof(value->licence) - used);
    }
}
#define ARCHIVE_TYPE UmiProductMarketplaceItemSnapshot
#define ARCHIVE_ENCODE umi_product_marketplace_snapshot_archive_encode
#define ARCHIVE_DECODE umi_product_marketplace_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_product_marketplace_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_product_marketplace_registry_archive_restore
#include "snapshot_contract_cases.h"
