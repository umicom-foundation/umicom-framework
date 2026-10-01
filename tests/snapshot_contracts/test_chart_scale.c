/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_scale.c
 * PURPOSE: Check chart scale input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/scale.h"
#include "umicom/chart/scale.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartScaleSnapshot
#define CONTRACT_REGISTRY UmiChartScaleRegistry
#define CONTRACT_CAPACITY UMI_CHART_SCALE_CAPACITY
#define CONTRACT_VALIDATE umi_chart_scale_snapshot_validate
#define CONTRACT_BATCH umi_chart_scale_registry_upsert_many
#define CONTRACT_CREATE umi_chart_scale_registry_create
#define CONTRACT_DESTROY umi_chart_scale_registry_destroy
#define CONTRACT_UPSERT umi_chart_scale_registry_upsert
#define CONTRACT_REMOVE umi_chart_scale_registry_remove
#define CONTRACT_FIND umi_chart_scale_registry_find
#define CONTRACT_AT umi_chart_scale_registry_at
#define CONTRACT_COUNT umi_chart_scale_registry_count
#define CONTRACT_REVISION umi_chart_scale_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartScaleSnapshot, id), sizeof(((UmiChartScaleSnapshot *)0)->id), 1 },
    {"pane_id", offsetof(UmiChartScaleSnapshot, pane_id), sizeof(((UmiChartScaleSnapshot *)0)->pane_id), 0 },
    {"side", offsetof(UmiChartScaleSnapshot, side), sizeof(((UmiChartScaleSnapshot *)0)->side), 0 }
};
static int ContractSnapshotEqual(const UmiChartScaleSnapshot *left, const UmiChartScaleSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->pane_id, right->pane_id, sizeof(left->pane_id)) == 0 &&
        memcmp(left->side, right->side, sizeof(left->side)) == 0 &&
        left->minimum == right->minimum &&
        left->maximum == right->maximum &&
        left->margin_top == right->margin_top &&
        left->margin_bottom == right->margin_bottom &&
        left->auto_scale == right->auto_scale &&
        left->logarithmic == right->logarithmic &&
        left->inverted == right->inverted &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartScaleSnapshot *item)
{
    item->pane_id[0] = 'v';
    item->side[0] = 'v';
    item->minimum = (double)6U;
    item->maximum = (double)7U;
    item->margin_top = (double)8U;
    item->margin_bottom = (double)9U;
    item->auto_scale = (int)10U;
    item->logarithmic = (int)11U;
    item->inverted = (int)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_scale_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_scale_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiChartScaleEdit
#define CONTRACT_EDIT_CURRENT umi_chart_scale_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_chart_scale_registry_read_page
#include "snapshot_contract_cases.h"
