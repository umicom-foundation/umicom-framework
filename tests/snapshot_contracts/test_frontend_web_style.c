/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_web_style.c
 * PURPOSE: Exercise frontend web_style snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/web_style.h"
#include "umicom/frontend/web_style.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendStyleSnapshot
#define CONTRACT_REGISTRY UmiFrontendStyleRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_WEB_STYLE_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_web_style_snapshot_validate
#define CONTRACT_BATCH umi_frontend_web_style_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_web_style_registry_create
#define CONTRACT_DESTROY umi_frontend_web_style_registry_destroy
#define CONTRACT_UPSERT umi_frontend_web_style_registry_upsert
#define CONTRACT_REMOVE umi_frontend_web_style_registry_remove
#define CONTRACT_FIND umi_frontend_web_style_registry_find
#define CONTRACT_AT umi_frontend_web_style_registry_at
#define CONTRACT_COUNT umi_frontend_web_style_registry_count
#define CONTRACT_REVISION umi_frontend_web_style_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendStyleSnapshot, id), sizeof(((UmiFrontendStyleSnapshot *)0)->id), 1 },
    {"selector", offsetof(UmiFrontendStyleSnapshot, selector), sizeof(((UmiFrontendStyleSnapshot *)0)->selector), 0 },
    {"property", offsetof(UmiFrontendStyleSnapshot, property), sizeof(((UmiFrontendStyleSnapshot *)0)->property), 0 },
    {"value", offsetof(UmiFrontendStyleSnapshot, value), sizeof(((UmiFrontendStyleSnapshot *)0)->value), 0 },
    {"media_query", offsetof(UmiFrontendStyleSnapshot, media_query), sizeof(((UmiFrontendStyleSnapshot *)0)->media_query), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendStyleSnapshot *left,
    const UmiFrontendStyleSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->selector, right->selector, sizeof(left->selector)) == 0 &&
        memcmp(left->property, right->property, sizeof(left->property)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->media_query, right->media_query, sizeof(left->media_query)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendStyleSnapshot *item)
{
    item->selector[0] = 'v';
    item->property[0] = 'v';
    item->value[0] = 'v';
    item->media_query[0] = 'v';
    item->order = (int32_t)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_web_style_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_web_style_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendStyleEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_web_style_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_frontend_web_style_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFrontendStyleSnapshot ArchiveSample(void)
{
    UmiFrontendStyleSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFrontendStyleSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->selector) + 1U;
        memset(value->selector + used, 0xa5, sizeof(value->selector) - used);
    }
    {
        size_t used = strlen(value->property) + 1U;
        memset(value->property + used, 0xa5, sizeof(value->property) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
    {
        size_t used = strlen(value->media_query) + 1U;
        memset(value->media_query + used, 0xa5, sizeof(value->media_query) - used);
    }
}
#define ARCHIVE_TYPE UmiFrontendStyleSnapshot
#define ARCHIVE_ENCODE umi_frontend_web_style_snapshot_archive_encode
#define ARCHIVE_DECODE umi_frontend_web_style_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_frontend_web_style_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_frontend_web_style_registry_archive_restore
#include "snapshot_contract_cases.h"
