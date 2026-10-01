/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_product_update_policy.c
 * PURPOSE: Check product update_policy input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/product/update_policy.h"
#include "umicom/product/update_policy.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProductUpdatePolicySnapshot
#define CONTRACT_REGISTRY UmiProductUpdatePolicyRegistry
#define CONTRACT_CAPACITY UMI_PRODUCT_UPDATE_POLICY_CAPACITY
#define CONTRACT_VALIDATE umi_product_update_policy_snapshot_validate
#define CONTRACT_BATCH umi_product_update_policy_registry_upsert_many
#define CONTRACT_CREATE umi_product_update_policy_registry_create
#define CONTRACT_DESTROY umi_product_update_policy_registry_destroy
#define CONTRACT_UPSERT umi_product_update_policy_registry_upsert
#define CONTRACT_REMOVE umi_product_update_policy_registry_remove
#define CONTRACT_FIND umi_product_update_policy_registry_find
#define CONTRACT_AT umi_product_update_policy_registry_at
#define CONTRACT_COUNT umi_product_update_policy_registry_count
#define CONTRACT_REVISION umi_product_update_policy_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProductUpdatePolicySnapshot, id), sizeof(((UmiProductUpdatePolicySnapshot *)0)->id), 1 },
    {"product_id", offsetof(UmiProductUpdatePolicySnapshot, product_id), sizeof(((UmiProductUpdatePolicySnapshot *)0)->product_id), 0 },
    {"channel", offsetof(UmiProductUpdatePolicySnapshot, channel), sizeof(((UmiProductUpdatePolicySnapshot *)0)->channel), 0 },
    {"allowed_range", offsetof(UmiProductUpdatePolicySnapshot, allowed_range), sizeof(((UmiProductUpdatePolicySnapshot *)0)->allowed_range), 0 }
};
static int ContractSnapshotEqual(const UmiProductUpdatePolicySnapshot *left, const UmiProductUpdatePolicySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->product_id, right->product_id, sizeof(left->product_id)) == 0 &&
        memcmp(left->channel, right->channel, sizeof(left->channel)) == 0 &&
        memcmp(left->allowed_range, right->allowed_range, sizeof(left->allowed_range)) == 0 &&
        left->automatic == right->automatic &&
        left->security_only == right->security_only &&
        left->allow_prerelease == right->allow_prerelease &&
        left->require_signature == right->require_signature &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiProductUpdatePolicySnapshot *item)
{
    item->product_id[0] = 'v';
    item->channel[0] = 'v';
    item->allowed_range[0] = 'v';
    item->automatic = (int)7U;
    item->security_only = (int)8U;
    item->allow_prerelease = (int)9U;
    item->require_signature = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_product_update_policy_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_product_update_policy_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProductUpdatePolicyEdit
#define CONTRACT_EDIT_CURRENT umi_product_update_policy_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_product_update_policy_registry_read_page
#include "snapshot_contract_cases.h"
