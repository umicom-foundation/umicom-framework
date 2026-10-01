/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_symbol.c
 * PURPOSE: Check language symbol input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/symbol.h"
#include "umicom/language/symbol.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageSymbolSnapshot
#define CONTRACT_REGISTRY UmiLanguageSymbolRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_SYMBOL_CAPACITY
#define CONTRACT_VALIDATE umi_language_symbol_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_symbol_registry_replace_document
#define CONTRACT_BATCH umi_language_symbol_registry_upsert_many
#define CONTRACT_CREATE umi_language_symbol_registry_create
#define CONTRACT_DESTROY umi_language_symbol_registry_destroy
#define CONTRACT_UPSERT umi_language_symbol_registry_upsert
#define CONTRACT_REMOVE umi_language_symbol_registry_remove
#define CONTRACT_FIND umi_language_symbol_registry_find
#define CONTRACT_AT umi_language_symbol_registry_at
#define CONTRACT_COUNT umi_language_symbol_registry_count
#define CONTRACT_REVISION umi_language_symbol_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageSymbolSnapshot, id), sizeof(((UmiLanguageSymbolSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageSymbolSnapshot, document_id), sizeof(((UmiLanguageSymbolSnapshot *)0)->document_id), 0 },
    {"name", offsetof(UmiLanguageSymbolSnapshot, name), sizeof(((UmiLanguageSymbolSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiLanguageSymbolSnapshot, kind), sizeof(((UmiLanguageSymbolSnapshot *)0)->kind), 0 },
    {"container", offsetof(UmiLanguageSymbolSnapshot, container), sizeof(((UmiLanguageSymbolSnapshot *)0)->container), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageSymbolSnapshot *left, const UmiLanguageSymbolSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->container, right->container, sizeof(left->container)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->end_line == right->end_line &&
        left->end_column == right->end_column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageSymbolSnapshot *item)
{
    item->document_id[0] = 'v';
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->container[0] = 'v';
    item->line = (uint32_t)8U;
    item->column = (uint32_t)9U;
    item->end_line = (uint32_t)10U;
    item->end_column = (uint32_t)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_symbol_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_symbol_registry_replace_if_current
#include "snapshot_contract_cases.h"
