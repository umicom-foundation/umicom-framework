/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_stream.c
 * PURPOSE: Check chart stream input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/stream.h"
#include "umicom/chart/stream.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartStreamSnapshot
#define CONTRACT_REGISTRY UmiChartStreamRegistry
#define CONTRACT_CAPACITY UMI_CHART_STREAM_CAPACITY
#define CONTRACT_VALIDATE umi_chart_stream_snapshot_validate
#define CONTRACT_BATCH umi_chart_stream_registry_upsert_many
#define CONTRACT_CREATE umi_chart_stream_registry_create
#define CONTRACT_DESTROY umi_chart_stream_registry_destroy
#define CONTRACT_UPSERT umi_chart_stream_registry_upsert
#define CONTRACT_REMOVE umi_chart_stream_registry_remove
#define CONTRACT_FIND umi_chart_stream_registry_find
#define CONTRACT_AT umi_chart_stream_registry_at
#define CONTRACT_COUNT umi_chart_stream_registry_count
#define CONTRACT_REVISION umi_chart_stream_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartStreamSnapshot, id), sizeof(((UmiChartStreamSnapshot *)0)->id), 1 },
    {"series_id", offsetof(UmiChartStreamSnapshot, series_id), sizeof(((UmiChartStreamSnapshot *)0)->series_id), 0 }
};
static int ContractSnapshotEqual(const UmiChartStreamSnapshot *left, const UmiChartStreamSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->series_id, right->series_id, sizeof(left->series_id)) == 0 &&
        left->updates == right->updates &&
        left->dropped == right->dropped &&
        left->last_time == right->last_time &&
        left->last_value == right->last_value &&
        left->connected == right->connected &&
        left->paused == right->paused &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartStreamSnapshot *item)
{
    item->series_id[0] = 'v';
    item->updates = (uint64_t)5U;
    item->dropped = (uint64_t)6U;
    item->last_time = (int64_t)7U;
    item->last_value = (double)8U;
    item->connected = (int)9U;
    item->paused = (int)10U;
}
#include "snapshot_contract_cases.h"
