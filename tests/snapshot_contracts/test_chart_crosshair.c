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
#include "snapshot_contract_cases.h"
