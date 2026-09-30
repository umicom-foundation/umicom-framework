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
}
#include "snapshot_contract_cases.h"
