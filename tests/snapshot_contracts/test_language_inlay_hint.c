/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_inlay_hint.c
 * PURPOSE: Check language inlay_hint input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/inlay_hint.h"
#include "umicom/language/inlay_hint.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageInlayHintSnapshot
#define CONTRACT_REGISTRY UmiLanguageInlayHintRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_INLAY_HINT_CAPACITY
#define CONTRACT_VALIDATE umi_language_inlay_hint_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_inlay_hint_registry_replace_document
#define CONTRACT_BATCH umi_language_inlay_hint_registry_upsert_many
#define CONTRACT_CREATE umi_language_inlay_hint_registry_create
#define CONTRACT_DESTROY umi_language_inlay_hint_registry_destroy
#define CONTRACT_UPSERT umi_language_inlay_hint_registry_upsert
#define CONTRACT_REMOVE umi_language_inlay_hint_registry_remove
#define CONTRACT_FIND umi_language_inlay_hint_registry_find
#define CONTRACT_AT umi_language_inlay_hint_registry_at
#define CONTRACT_COUNT umi_language_inlay_hint_registry_count
#define CONTRACT_REVISION umi_language_inlay_hint_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageInlayHintSnapshot, id), sizeof(((UmiLanguageInlayHintSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageInlayHintSnapshot, document_id), sizeof(((UmiLanguageInlayHintSnapshot *)0)->document_id), 0 },
    {"label", offsetof(UmiLanguageInlayHintSnapshot, label), sizeof(((UmiLanguageInlayHintSnapshot *)0)->label), 0 },
    {"kind", offsetof(UmiLanguageInlayHintSnapshot, kind), sizeof(((UmiLanguageInlayHintSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageInlayHintSnapshot *left, const UmiLanguageInlayHintSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->visible == right->visible &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageInlayHintSnapshot *item)
{
    item->document_id[0] = 'v';
    item->label[0] = 'v';
    item->kind[0] = 'v';
    item->line = (uint32_t)7U;
    item->column = (uint32_t)8U;
    item->visible = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_inlay_hint_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_inlay_hint_registry_replace_if_current
#include "snapshot_contract_cases.h"
