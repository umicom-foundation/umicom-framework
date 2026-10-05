/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_sort_filter_model.c
 * PURPOSE: Exercise ui sort_filter_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/sort_filter_model.h"
#include "umicom/ui/sort_filter_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiSortFilterSnapshot
#define CONTRACT_REGISTRY UmiUiSortFilterRegistry
#define CONTRACT_CAPACITY UMI_UI_SORT_FILTER_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_sort_filter_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_sort_filter_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_sort_filter_model_registry_create
#define CONTRACT_DESTROY umi_ui_sort_filter_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_sort_filter_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_sort_filter_model_registry_remove
#define CONTRACT_FIND umi_ui_sort_filter_model_registry_find
#define CONTRACT_AT umi_ui_sort_filter_model_registry_at
#define CONTRACT_COUNT umi_ui_sort_filter_model_registry_count
#define CONTRACT_REVISION umi_ui_sort_filter_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiSortFilterSnapshot, id), sizeof(((UmiUiSortFilterSnapshot *)0)->id), 1 },
    {"query", offsetof(UmiUiSortFilterSnapshot, query), sizeof(((UmiUiSortFilterSnapshot *)0)->query), 0 },
    {"sort_key", offsetof(UmiUiSortFilterSnapshot, sort_key), sizeof(((UmiUiSortFilterSnapshot *)0)->sort_key), 0 }
};
static int ContractSnapshotEqual(const UmiUiSortFilterSnapshot *left,
    const UmiUiSortFilterSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->query, right->query, sizeof(left->query)) == 0 &&
        memcmp(left->sort_key, right->sort_key, sizeof(left->sort_key)) == 0 &&
        left->ascending == right->ascending &&
        left->case_sensitive == right->case_sensitive &&
        left->enabled == right->enabled &&
        left->priority == right->priority &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiSortFilterSnapshot *item)
{
    item->query[0] = 'v';
    item->sort_key[0] = 'v';
    item->ascending = (int)6U;
    item->case_sensitive = (int)7U;
    item->enabled = (int)8U;
    item->priority = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_sort_filter_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_sort_filter_model_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiSortFilterEdit
#define CONTRACT_EDIT_CURRENT umi_ui_sort_filter_model_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_sort_filter_model_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiSortFilterSnapshot ArchiveSample(void)
{
    UmiUiSortFilterSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiSortFilterSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->query) + 1U;
        memset(value->query + used, 0xa5, sizeof(value->query) - used);
    }
    {
        size_t used = strlen(value->sort_key) + 1U;
        memset(value->sort_key + used, 0xa5, sizeof(value->sort_key) - used);
    }
}
#define ARCHIVE_TYPE UmiUiSortFilterSnapshot
#define ARCHIVE_ENCODE umi_ui_sort_filter_model_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_sort_filter_model_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_sort_filter_model_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_sort_filter_model_registry_archive_restore
#include "snapshot_contract_cases.h"
