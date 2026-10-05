/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_crosshair.c
 * PURPOSE: Check chart crosshair input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/crosshair.h"
#include "umicom/chart/crosshair.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartCrosshairSnapshot
#define CONTRACT_REGISTRY UmiChartCrosshairRegistry
#define CONTRACT_CAPACITY UMI_CHART_CROSSHAIR_CAPACITY
#define CONTRACT_VALIDATE umi_chart_crosshair_snapshot_validate
#define CONTRACT_BATCH umi_chart_crosshair_registry_upsert_many
#define CONTRACT_CREATE umi_chart_crosshair_registry_create
#define CONTRACT_DESTROY umi_chart_crosshair_registry_destroy
#define CONTRACT_UPSERT umi_chart_crosshair_registry_upsert
#define CONTRACT_REMOVE umi_chart_crosshair_registry_remove
#define CONTRACT_FIND umi_chart_crosshair_registry_find
#define CONTRACT_AT umi_chart_crosshair_registry_at
#define CONTRACT_COUNT umi_chart_crosshair_registry_count
#define CONTRACT_REVISION umi_chart_crosshair_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartCrosshairSnapshot, id), sizeof(((UmiChartCrosshairSnapshot *)0)->id), 1 },
    {"pane_id", offsetof(UmiChartCrosshairSnapshot, pane_id), sizeof(((UmiChartCrosshairSnapshot *)0)->pane_id), 0 }
};
static int ContractSnapshotEqual(const UmiChartCrosshairSnapshot *left, const UmiChartCrosshairSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->pane_id, right->pane_id, sizeof(left->pane_id)) == 0 &&
        left->time == right->time &&
        left->value == right->value &&
        left->visible == right->visible &&
        left->magnet == right->magnet &&
        left->show_labels == right->show_labels &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartCrosshairSnapshot *item)
{
    item->pane_id[0] = 'v';
    item->time = (int64_t)5U;
    item->value = (double)6U;
    item->visible = (int)7U;
    item->magnet = (int)8U;
    item->show_labels = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_crosshair_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_crosshair_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiChartCrosshairEdit
#define CONTRACT_EDIT_CURRENT umi_chart_crosshair_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_chart_crosshair_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiChartCrosshairSnapshot ArchiveSample(void)
{
    UmiChartCrosshairSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiChartCrosshairSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->pane_id) + 1U;
        memset(value->pane_id + used, 0xa5, sizeof(value->pane_id) - used);
    }
}
#define ARCHIVE_TYPE UmiChartCrosshairSnapshot
#define ARCHIVE_ENCODE umi_chart_crosshair_snapshot_archive_encode
#define ARCHIVE_DECODE umi_chart_crosshair_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_chart_crosshair_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_chart_crosshair_registry_archive_restore
#include "snapshot_contract_cases.h"
