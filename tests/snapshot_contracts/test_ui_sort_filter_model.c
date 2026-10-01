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
#include "snapshot_contract_cases.h"
