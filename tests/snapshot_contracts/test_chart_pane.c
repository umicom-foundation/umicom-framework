/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_pane.c
 * PURPOSE: Check chart pane input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/pane.h"
#include "umicom/chart/pane.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartPaneSnapshot
#define CONTRACT_REGISTRY UmiChartPaneRegistry
#define CONTRACT_CAPACITY UMI_CHART_PANE_CAPACITY
#define CONTRACT_VALIDATE umi_chart_pane_snapshot_validate
#define CONTRACT_BATCH umi_chart_pane_registry_upsert_many
#define CONTRACT_CREATE umi_chart_pane_registry_create
#define CONTRACT_DESTROY umi_chart_pane_registry_destroy
#define CONTRACT_UPSERT umi_chart_pane_registry_upsert
#define CONTRACT_REMOVE umi_chart_pane_registry_remove
#define CONTRACT_FIND umi_chart_pane_registry_find
#define CONTRACT_AT umi_chart_pane_registry_at
#define CONTRACT_COUNT umi_chart_pane_registry_count
#define CONTRACT_REVISION umi_chart_pane_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartPaneSnapshot, id), sizeof(((UmiChartPaneSnapshot *)0)->id), 1 },
    {"title", offsetof(UmiChartPaneSnapshot, title), sizeof(((UmiChartPaneSnapshot *)0)->title), 0 }
};
static int ContractSnapshotEqual(const UmiChartPaneSnapshot *left, const UmiChartPaneSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        left->height_weight == right->height_weight &&
        left->visible == right->visible &&
        left->collapsed == right->collapsed &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartPaneSnapshot *item)
{
    item->title[0] = 'v';
    item->height_weight = (double)5U;
    item->visible = (int)6U;
    item->collapsed = (int)7U;
    item->order = (int32_t)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_pane_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_pane_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiChartPaneEdit
#define CONTRACT_EDIT_CURRENT umi_chart_pane_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_chart_pane_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiChartPaneSnapshot ArchiveSample(void)
{
    UmiChartPaneSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiChartPaneSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
}
#define ARCHIVE_TYPE UmiChartPaneSnapshot
#define ARCHIVE_ENCODE umi_chart_pane_snapshot_archive_encode
#define ARCHIVE_DECODE umi_chart_pane_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_chart_pane_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_chart_pane_registry_archive_restore
#include "snapshot_contract_cases.h"
