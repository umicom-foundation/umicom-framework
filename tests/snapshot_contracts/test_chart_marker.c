/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_marker.c
 * PURPOSE: Check chart marker input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/marker.h"
#include "umicom/chart/marker.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartMarkerSnapshot
#define CONTRACT_REGISTRY UmiChartMarkerRegistry
#define CONTRACT_CAPACITY UMI_CHART_MARKER_CAPACITY
#define CONTRACT_VALIDATE umi_chart_marker_snapshot_validate
#define CONTRACT_BATCH umi_chart_marker_registry_upsert_many
#define CONTRACT_CREATE umi_chart_marker_registry_create
#define CONTRACT_DESTROY umi_chart_marker_registry_destroy
#define CONTRACT_UPSERT umi_chart_marker_registry_upsert
#define CONTRACT_REMOVE umi_chart_marker_registry_remove
#define CONTRACT_FIND umi_chart_marker_registry_find
#define CONTRACT_AT umi_chart_marker_registry_at
#define CONTRACT_COUNT umi_chart_marker_registry_count
#define CONTRACT_REVISION umi_chart_marker_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartMarkerSnapshot, id), sizeof(((UmiChartMarkerSnapshot *)0)->id), 1 },
    {"series_id", offsetof(UmiChartMarkerSnapshot, series_id), sizeof(((UmiChartMarkerSnapshot *)0)->series_id), 0 },
    {"text", offsetof(UmiChartMarkerSnapshot, text), sizeof(((UmiChartMarkerSnapshot *)0)->text), 0 },
    {"shape", offsetof(UmiChartMarkerSnapshot, shape), sizeof(((UmiChartMarkerSnapshot *)0)->shape), 0 },
    {"position", offsetof(UmiChartMarkerSnapshot, position), sizeof(((UmiChartMarkerSnapshot *)0)->position), 0 }
};
static int ContractSnapshotEqual(const UmiChartMarkerSnapshot *left, const UmiChartMarkerSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->series_id, right->series_id, sizeof(left->series_id)) == 0 &&
        left->time == right->time &&
        left->value == right->value &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        memcmp(left->shape, right->shape, sizeof(left->shape)) == 0 &&
        memcmp(left->position, right->position, sizeof(left->position)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartMarkerSnapshot *item)
{
    item->series_id[0] = 'v';
    item->time = (int64_t)5U;
    item->value = (double)6U;
    item->text[0] = 'v';
    item->shape[0] = 'v';
    item->position[0] = 'v';
    item->order = (int32_t)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_chart_marker_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_chart_marker_registry_replace_if_current
#include "snapshot_contract_cases.h"
