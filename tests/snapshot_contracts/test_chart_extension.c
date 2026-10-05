/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_extension.c
 * PURPOSE: Check chart extension input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/extension.h"
#include "umicom/chart/extension.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartExtensionSnapshot
#define CONTRACT_REGISTRY UmiChartExtensionRegistry
#define CONTRACT_CAPACITY UMI_CHART_EXTENSION_CAPACITY
#define CONTRACT_VALIDATE umi_chart_extension_snapshot_validate
#define CONTRACT_BATCH umi_chart_extension_registry_upsert_many
#define CONTRACT_CREATE umi_chart_extension_registry_create
#define CONTRACT_DESTROY umi_chart_extension_registry_destroy
#define CONTRACT_UPSERT umi_chart_extension_registry_upsert
#define CONTRACT_REMOVE umi_chart_extension_registry_remove
#define CONTRACT_FIND umi_chart_extension_registry_find
#define CONTRACT_AT umi_chart_extension_registry_at
#define CONTRACT_COUNT umi_chart_extension_registry_count
#define CONTRACT_REVISION umi_chart_extension_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartExtensionSnapshot, id), sizeof(((UmiChartExtensionSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiChartExtensionSnapshot, name), sizeof(((UmiChartExtensionSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiChartExtensionSnapshot, kind), sizeof(((UmiChartExtensionSnapshot *)0)->kind), 0 },
    {"provider_id", offsetof(UmiChartExtensionSnapshot, provider_id), sizeof(((UmiChartExtensionSnapshot *)0)->provider_id), 0 },
    {"entry_point", offsetof(UmiChartExtensionSnapshot, entry_point), sizeof(((UmiChartExtensionSnapshot *)0)->entry_point), 0 }
};
static int ContractSnapshotEqual(const UmiChartExtensionSnapshot *left, const UmiChartExtensionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->provider_id, right->provider_id, sizeof(left->provider_id)) == 0 &&
        memcmp(left->entry_point, right->entry_point, sizeof(left->entry_point)) == 0 &&
        left->enabled == right->enabled &&
        left->trusted == right->trusted &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartExtensionSnapshot *item)
{
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->provider_id[0] = 'v';
    item->entry_point[0] = 'v';
    item->enabled = (int)8U;
    item->trusted = (int)9U;
    item->order = (int32_t)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_extension_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_extension_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiChartExtensionEdit
#define CONTRACT_EDIT_CURRENT umi_chart_extension_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_chart_extension_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiChartExtensionSnapshot ArchiveSample(void)
{
    UmiChartExtensionSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiChartExtensionSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->entry_point) + 1U;
        memset(value->entry_point + used, 0xa5, sizeof(value->entry_point) - used);
    }
}
#define ARCHIVE_TYPE UmiChartExtensionSnapshot
#define ARCHIVE_ENCODE umi_chart_extension_snapshot_archive_encode
#define ARCHIVE_DECODE umi_chart_extension_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_chart_extension_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_chart_extension_registry_archive_restore
#include "snapshot_contract_cases.h"
