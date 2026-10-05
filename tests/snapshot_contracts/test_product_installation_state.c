/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_product_installation_state.c
 * PURPOSE: Check product installation_state input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/product/installation_state.h"
#include "umicom/product/installation_state.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProductInstallationSnapshot
#define CONTRACT_REGISTRY UmiProductInstallationRegistry
#define CONTRACT_CAPACITY UMI_PRODUCT_INSTALLATION_STATE_CAPACITY
#define CONTRACT_VALIDATE umi_product_installation_state_snapshot_validate
#define CONTRACT_BATCH umi_product_installation_state_registry_upsert_many
#define CONTRACT_CREATE umi_product_installation_state_registry_create
#define CONTRACT_DESTROY umi_product_installation_state_registry_destroy
#define CONTRACT_UPSERT umi_product_installation_state_registry_upsert
#define CONTRACT_REMOVE umi_product_installation_state_registry_remove
#define CONTRACT_FIND umi_product_installation_state_registry_find
#define CONTRACT_AT umi_product_installation_state_registry_at
#define CONTRACT_COUNT umi_product_installation_state_registry_count
#define CONTRACT_REVISION umi_product_installation_state_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProductInstallationSnapshot, id), sizeof(((UmiProductInstallationSnapshot *)0)->id), 1 },
    {"product_id", offsetof(UmiProductInstallationSnapshot, product_id), sizeof(((UmiProductInstallationSnapshot *)0)->product_id), 0 },
    {"version", offsetof(UmiProductInstallationSnapshot, version), sizeof(((UmiProductInstallationSnapshot *)0)->version), 0 },
    {"install_root", offsetof(UmiProductInstallationSnapshot, install_root), sizeof(((UmiProductInstallationSnapshot *)0)->install_root), 0 },
    {"channel", offsetof(UmiProductInstallationSnapshot, channel), sizeof(((UmiProductInstallationSnapshot *)0)->channel), 0 }
};
static int ContractSnapshotEqual(const UmiProductInstallationSnapshot *left, const UmiProductInstallationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->product_id, right->product_id, sizeof(left->product_id)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        memcmp(left->install_root, right->install_root, sizeof(left->install_root)) == 0 &&
        memcmp(left->channel, right->channel, sizeof(left->channel)) == 0 &&
        left->installed_at == right->installed_at &&
        left->state == right->state &&
        left->verified == right->verified &&
        left->rollback_available == right->rollback_available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiProductInstallationSnapshot *item)
{
    item->product_id[0] = 'v';
    item->version[0] = 'v';
    item->install_root[0] = 'v';
    item->channel[0] = 'v';
    item->installed_at = (uint64_t)8U;
    item->state = (int)9U;
    item->verified = (int)10U;
    item->rollback_available = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_product_installation_state_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_product_installation_state_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProductInstallationEdit
#define CONTRACT_EDIT_CURRENT umi_product_installation_state_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_product_installation_state_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProductInstallationSnapshot ArchiveSample(void)
{
    UmiProductInstallationSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProductInstallationSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->product_id) + 1U;
        memset(value->product_id + used, 0xa5, sizeof(value->product_id) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->install_root) + 1U;
        memset(value->install_root + used, 0xa5, sizeof(value->install_root) - used);
    }
    {
        size_t used = strlen(value->channel) + 1U;
        memset(value->channel + used, 0xa5, sizeof(value->channel) - used);
    }
}
#define ARCHIVE_TYPE UmiProductInstallationSnapshot
#define ARCHIVE_ENCODE umi_product_installation_state_snapshot_archive_encode
#define ARCHIVE_DECODE umi_product_installation_state_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_product_installation_state_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_product_installation_state_registry_archive_restore
#include "snapshot_contract_cases.h"
