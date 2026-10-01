/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_provider.c
 * PURPOSE: Check language provider input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/provider.h"
#include "umicom/language/provider.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageProviderSnapshot
#define CONTRACT_REGISTRY UmiLanguageProviderRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_PROVIDER_CAPACITY
#define CONTRACT_VALIDATE umi_language_provider_snapshot_validate
#define CONTRACT_BATCH umi_language_provider_registry_upsert_many
#define CONTRACT_CREATE umi_language_provider_registry_create
#define CONTRACT_DESTROY umi_language_provider_registry_destroy
#define CONTRACT_UPSERT umi_language_provider_registry_upsert
#define CONTRACT_REMOVE umi_language_provider_registry_remove
#define CONTRACT_FIND umi_language_provider_registry_find
#define CONTRACT_AT umi_language_provider_registry_at
#define CONTRACT_COUNT umi_language_provider_registry_count
#define CONTRACT_REVISION umi_language_provider_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageProviderSnapshot, id), sizeof(((UmiLanguageProviderSnapshot *)0)->id), 1 },
    {"language_id", offsetof(UmiLanguageProviderSnapshot, language_id), sizeof(((UmiLanguageProviderSnapshot *)0)->language_id), 0 },
    {"kind", offsetof(UmiLanguageProviderSnapshot, kind), sizeof(((UmiLanguageProviderSnapshot *)0)->kind), 0 },
    {"name", offsetof(UmiLanguageProviderSnapshot, name), sizeof(((UmiLanguageProviderSnapshot *)0)->name), 0 },
    {"command", offsetof(UmiLanguageProviderSnapshot, command), sizeof(((UmiLanguageProviderSnapshot *)0)->command), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageProviderSnapshot *left, const UmiLanguageProviderSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->language_id, right->language_id, sizeof(left->language_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->command, right->command, sizeof(left->command)) == 0 &&
        left->priority == right->priority &&
        left->enabled == right->enabled &&
        left->healthy == right->healthy &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageProviderSnapshot *item)
{
    item->language_id[0] = 'v';
    item->kind[0] = 'v';
    item->name[0] = 'v';
    item->command[0] = 'v';
    item->priority = (int32_t)8U;
    item->enabled = (int)9U;
    item->healthy = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_provider_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_provider_registry_replace_if_current
#include "snapshot_contract_cases.h"
