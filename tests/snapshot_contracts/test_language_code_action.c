/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_code_action.c
 * PURPOSE: Check language code_action input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/code_action.h"
#include "umicom/language/code_action.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageCodeActionSnapshot
#define CONTRACT_REGISTRY UmiLanguageCodeActionRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_CODE_ACTION_CAPACITY
#define CONTRACT_VALIDATE umi_language_code_action_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_code_action_registry_replace_document
#define CONTRACT_BATCH umi_language_code_action_registry_upsert_many
#define CONTRACT_CREATE umi_language_code_action_registry_create
#define CONTRACT_DESTROY umi_language_code_action_registry_destroy
#define CONTRACT_UPSERT umi_language_code_action_registry_upsert
#define CONTRACT_REMOVE umi_language_code_action_registry_remove
#define CONTRACT_FIND umi_language_code_action_registry_find
#define CONTRACT_AT umi_language_code_action_registry_at
#define CONTRACT_COUNT umi_language_code_action_registry_count
#define CONTRACT_REVISION umi_language_code_action_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageCodeActionSnapshot, id), sizeof(((UmiLanguageCodeActionSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageCodeActionSnapshot, document_id), sizeof(((UmiLanguageCodeActionSnapshot *)0)->document_id), 0 },
    {"title", offsetof(UmiLanguageCodeActionSnapshot, title), sizeof(((UmiLanguageCodeActionSnapshot *)0)->title), 0 },
    {"kind", offsetof(UmiLanguageCodeActionSnapshot, kind), sizeof(((UmiLanguageCodeActionSnapshot *)0)->kind), 0 },
    {"command_id", offsetof(UmiLanguageCodeActionSnapshot, command_id), sizeof(((UmiLanguageCodeActionSnapshot *)0)->command_id), 0 },
    {"argument", offsetof(UmiLanguageCodeActionSnapshot, argument), sizeof(((UmiLanguageCodeActionSnapshot *)0)->argument), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageCodeActionSnapshot *left, const UmiLanguageCodeActionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->argument, right->argument, sizeof(left->argument)) == 0 &&
        left->preferred == right->preferred &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageCodeActionSnapshot *item)
{
    item->document_id[0] = 'v';
    item->title[0] = 'v';
    item->kind[0] = 'v';
    item->command_id[0] = 'v';
    item->argument[0] = 'v';
    item->preferred = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_code_action_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_code_action_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageCodeActionEdit
#define CONTRACT_EDIT_CURRENT umi_language_code_action_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_code_action_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiLanguageCodeActionSnapshot ArchiveSample(void)
{
    UmiLanguageCodeActionSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiLanguageCodeActionSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->document_id) + 1U;
        memset(value->document_id + used, 0xa5, sizeof(value->document_id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->argument) + 1U;
        memset(value->argument + used, 0xa5, sizeof(value->argument) - used);
    }
}
#define ARCHIVE_TYPE UmiLanguageCodeActionSnapshot
#define ARCHIVE_ENCODE umi_language_code_action_snapshot_archive_encode
#define ARCHIVE_DECODE umi_language_code_action_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_language_code_action_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_language_code_action_registry_archive_restore
#include "snapshot_contract_cases.h"
