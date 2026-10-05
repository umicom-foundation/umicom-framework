/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_drawing.c
 * PURPOSE: Check chart drawing input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/drawing.h"
#include "umicom/chart/drawing.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartDrawingSnapshot
#define CONTRACT_REGISTRY UmiChartDrawingRegistry
#define CONTRACT_CAPACITY UMI_CHART_DRAWING_CAPACITY
#define CONTRACT_VALIDATE umi_chart_drawing_snapshot_validate
#define CONTRACT_BATCH umi_chart_drawing_registry_upsert_many
#define CONTRACT_CREATE umi_chart_drawing_registry_create
#define CONTRACT_DESTROY umi_chart_drawing_registry_destroy
#define CONTRACT_UPSERT umi_chart_drawing_registry_upsert
#define CONTRACT_REMOVE umi_chart_drawing_registry_remove
#define CONTRACT_FIND umi_chart_drawing_registry_find
#define CONTRACT_AT umi_chart_drawing_registry_at
#define CONTRACT_COUNT umi_chart_drawing_registry_count
#define CONTRACT_REVISION umi_chart_drawing_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartDrawingSnapshot, id), sizeof(((UmiChartDrawingSnapshot *)0)->id), 1 },
    {"pane_id", offsetof(UmiChartDrawingSnapshot, pane_id), sizeof(((UmiChartDrawingSnapshot *)0)->pane_id), 0 },
    {"tool", offsetof(UmiChartDrawingSnapshot, tool), sizeof(((UmiChartDrawingSnapshot *)0)->tool), 0 },
    {"style", offsetof(UmiChartDrawingSnapshot, style), sizeof(((UmiChartDrawingSnapshot *)0)->style), 0 }
};
static int ContractSnapshotEqual(const UmiChartDrawingSnapshot *left, const UmiChartDrawingSnapshot *right)
{
    /* Compare the appended visibility separately; retain the original field
     * comparisons so the extended value contract exercises every old member. */
    if (left->visibility_flags != right->visibility_flags) return 0;
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->pane_id, right->pane_id, sizeof(left->pane_id)) == 0 &&
        memcmp(left->tool, right->tool, sizeof(left->tool)) == 0 &&
        left->time1 == right->time1 &&
        left->time2 == right->time2 &&
        left->value1 == right->value1 &&
        left->value2 == right->value2 &&
        memcmp(left->style, right->style, sizeof(left->style)) == 0 &&
        left->selected == right->selected &&
        left->locked == right->locked &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartDrawingSnapshot *item)
{
    item->pane_id[0] = 'v';
    item->tool[0] = 'v';
    item->time1 = (int64_t)6U;
    item->time2 = (int64_t)7U;
    item->value1 = (double)8U;
    item->value2 = (double)9U;
    item->style[0] = 'v';
    item->selected = (int)11U;
    item->locked = (int)12U;
    item->visibility_flags = UMI_CHART_DRAWING_VISIBILITY_HIDDEN;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_drawing_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_drawing_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiChartDrawingEdit
#define CONTRACT_EDIT_CURRENT umi_chart_drawing_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_chart_drawing_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiChartDrawingSnapshot ArchiveSample(void)
{
    UmiChartDrawingSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiChartDrawingSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->pane_id) + 1U;
        memset(value->pane_id + used, 0xa5, sizeof(value->pane_id) - used);
    }
    {
        size_t used = strlen(value->tool) + 1U;
        memset(value->tool + used, 0xa5, sizeof(value->tool) - used);
    }
    {
        size_t used = strlen(value->style) + 1U;
        memset(value->style + used, 0xa5, sizeof(value->style) - used);
    }
}
#define ARCHIVE_TYPE UmiChartDrawingSnapshot
#define ARCHIVE_ENCODE umi_chart_drawing_snapshot_archive_encode
#define ARCHIVE_DECODE umi_chart_drawing_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_chart_drawing_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_chart_drawing_registry_archive_restore
#include "snapshot_contract_cases.h"
