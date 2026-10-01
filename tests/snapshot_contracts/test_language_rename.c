/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_rename.c
 * PURPOSE: Check language rename input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/rename.h"
#include "umicom/language/rename.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageRenameSnapshot
#define CONTRACT_REGISTRY UmiLanguageRenameRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_RENAME_CAPACITY
#define CONTRACT_VALIDATE umi_language_rename_snapshot_validate
#define CONTRACT_BATCH umi_language_rename_registry_upsert_many
#define CONTRACT_CREATE umi_language_rename_registry_create
#define CONTRACT_DESTROY umi_language_rename_registry_destroy
#define CONTRACT_UPSERT umi_language_rename_registry_upsert
#define CONTRACT_REMOVE umi_language_rename_registry_remove
#define CONTRACT_FIND umi_language_rename_registry_find
#define CONTRACT_AT umi_language_rename_registry_at
#define CONTRACT_COUNT umi_language_rename_registry_count
#define CONTRACT_REVISION umi_language_rename_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageRenameSnapshot, id), sizeof(((UmiLanguageRenameSnapshot *)0)->id), 1 },
    {"symbol_id", offsetof(UmiLanguageRenameSnapshot, symbol_id), sizeof(((UmiLanguageRenameSnapshot *)0)->symbol_id), 0 },
    {"old_name", offsetof(UmiLanguageRenameSnapshot, old_name), sizeof(((UmiLanguageRenameSnapshot *)0)->old_name), 0 },
    {"new_name", offsetof(UmiLanguageRenameSnapshot, new_name), sizeof(((UmiLanguageRenameSnapshot *)0)->new_name), 0 },
    {"document_id", offsetof(UmiLanguageRenameSnapshot, document_id), sizeof(((UmiLanguageRenameSnapshot *)0)->document_id), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageRenameSnapshot *left, const UmiLanguageRenameSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->symbol_id, right->symbol_id, sizeof(left->symbol_id)) == 0 &&
        memcmp(left->old_name, right->old_name, sizeof(left->old_name)) == 0 &&
        memcmp(left->new_name, right->new_name, sizeof(left->new_name)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        left->state == right->state &&
        left->conflict_count == right->conflict_count &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageRenameSnapshot *item)
{
    item->symbol_id[0] = 'v';
    item->old_name[0] = 'v';
    item->new_name[0] = 'v';
    item->document_id[0] = 'v';
    item->state = (int)8U;
    item->conflict_count = (size_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_rename_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_rename_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageRenameEdit
#define CONTRACT_EDIT_CURRENT umi_language_rename_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_rename_registry_read_page
#include "snapshot_contract_cases.h"
