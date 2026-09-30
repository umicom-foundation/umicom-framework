/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_product_metadata_provider.c
 * PURPOSE: Check product metadata_provider input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/product/metadata_provider.h"
#include "umicom/product/metadata_provider.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProductMetadataProviderSnapshot
#define CONTRACT_REGISTRY UmiProductMetadataProviderRegistry
#define CONTRACT_CAPACITY UMI_PRODUCT_METADATA_PROVIDER_CAPACITY
#define CONTRACT_VALIDATE umi_product_metadata_provider_snapshot_validate
#define CONTRACT_BATCH umi_product_metadata_provider_registry_upsert_many
#define CONTRACT_CREATE umi_product_metadata_provider_registry_create
#define CONTRACT_DESTROY umi_product_metadata_provider_registry_destroy
#define CONTRACT_UPSERT umi_product_metadata_provider_registry_upsert
#define CONTRACT_REMOVE umi_product_metadata_provider_registry_remove
#define CONTRACT_FIND umi_product_metadata_provider_registry_find
#define CONTRACT_AT umi_product_metadata_provider_registry_at
#define CONTRACT_COUNT umi_product_metadata_provider_registry_count
#define CONTRACT_REVISION umi_product_metadata_provider_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProductMetadataProviderSnapshot, id), sizeof(((UmiProductMetadataProviderSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiProductMetadataProviderSnapshot, name), sizeof(((UmiProductMetadataProviderSnapshot *)0)->name), 0 },
    {"endpoint", offsetof(UmiProductMetadataProviderSnapshot, endpoint), sizeof(((UmiProductMetadataProviderSnapshot *)0)->endpoint), 0 },
    {"kind", offsetof(UmiProductMetadataProviderSnapshot, kind), sizeof(((UmiProductMetadataProviderSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiProductMetadataProviderSnapshot *left, const UmiProductMetadataProviderSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->endpoint, right->endpoint, sizeof(left->endpoint)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->enabled == right->enabled &&
        left->trusted == right->trusted &&
        left->priority == right->priority &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiProductMetadataProviderSnapshot *item)
{
    item->name[0] = 'v';
    item->endpoint[0] = 'v';
    item->kind[0] = 'v';
    item->enabled = (int)7U;
    item->trusted = (int)8U;
    item->priority = (int)9U;
}
#include "snapshot_contract_cases.h"
