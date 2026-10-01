/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_definition.c
 * PURPOSE: Check language definition input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/definition.h"
#include "umicom/language/definition.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageDefinitionSnapshot
#define CONTRACT_REGISTRY UmiLanguageDefinitionRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_DEFINITION_CAPACITY
#define CONTRACT_VALIDATE umi_language_definition_snapshot_validate
#define CONTRACT_BATCH umi_language_definition_registry_upsert_many
#define CONTRACT_CREATE umi_language_definition_registry_create
#define CONTRACT_DESTROY umi_language_definition_registry_destroy
#define CONTRACT_UPSERT umi_language_definition_registry_upsert
#define CONTRACT_REMOVE umi_language_definition_registry_remove
#define CONTRACT_FIND umi_language_definition_registry_find
#define CONTRACT_AT umi_language_definition_registry_at
#define CONTRACT_COUNT umi_language_definition_registry_count
#define CONTRACT_REVISION umi_language_definition_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageDefinitionSnapshot, id), sizeof(((UmiLanguageDefinitionSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiLanguageDefinitionSnapshot, name), sizeof(((UmiLanguageDefinitionSnapshot *)0)->name), 0 },
    {"file_extensions", offsetof(UmiLanguageDefinitionSnapshot, file_extensions), sizeof(((UmiLanguageDefinitionSnapshot *)0)->file_extensions), 0 },
    {"mime_types", offsetof(UmiLanguageDefinitionSnapshot, mime_types), sizeof(((UmiLanguageDefinitionSnapshot *)0)->mime_types), 0 },
    {"language_server", offsetof(UmiLanguageDefinitionSnapshot, language_server), sizeof(((UmiLanguageDefinitionSnapshot *)0)->language_server), 0 },
    {"formatter", offsetof(UmiLanguageDefinitionSnapshot, formatter), sizeof(((UmiLanguageDefinitionSnapshot *)0)->formatter), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageDefinitionSnapshot *left, const UmiLanguageDefinitionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->file_extensions, right->file_extensions, sizeof(left->file_extensions)) == 0 &&
        memcmp(left->mime_types, right->mime_types, sizeof(left->mime_types)) == 0 &&
        memcmp(left->language_server, right->language_server, sizeof(left->language_server)) == 0 &&
        memcmp(left->formatter, right->formatter, sizeof(left->formatter)) == 0 &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageDefinitionSnapshot *item)
{
    item->name[0] = 'v';
    item->file_extensions[0] = 'v';
    item->mime_types[0] = 'v';
    item->language_server[0] = 'v';
    item->formatter[0] = 'v';
    item->enabled = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_definition_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_definition_registry_replace_if_current
#include "snapshot_contract_cases.h"
