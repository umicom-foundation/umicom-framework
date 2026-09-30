/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_chart_annotation.c
 * PURPOSE: Check chart annotation input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/chart/annotation.h"
#include "umicom/chart/annotation.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiChartAnnotationSnapshot
#define CONTRACT_REGISTRY UmiChartAnnotationRegistry
#define CONTRACT_CAPACITY UMI_CHART_ANNOTATION_CAPACITY
#define CONTRACT_VALIDATE umi_chart_annotation_snapshot_validate
#define CONTRACT_BATCH umi_chart_annotation_registry_upsert_many
#define CONTRACT_CREATE umi_chart_annotation_registry_create
#define CONTRACT_DESTROY umi_chart_annotation_registry_destroy
#define CONTRACT_UPSERT umi_chart_annotation_registry_upsert
#define CONTRACT_REMOVE umi_chart_annotation_registry_remove
#define CONTRACT_FIND umi_chart_annotation_registry_find
#define CONTRACT_AT umi_chart_annotation_registry_at
#define CONTRACT_COUNT umi_chart_annotation_registry_count
#define CONTRACT_REVISION umi_chart_annotation_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiChartAnnotationSnapshot, id), sizeof(((UmiChartAnnotationSnapshot *)0)->id), 1 },
    {"pane_id", offsetof(UmiChartAnnotationSnapshot, pane_id), sizeof(((UmiChartAnnotationSnapshot *)0)->pane_id), 0 },
    {"kind", offsetof(UmiChartAnnotationSnapshot, kind), sizeof(((UmiChartAnnotationSnapshot *)0)->kind), 0 },
    {"text", offsetof(UmiChartAnnotationSnapshot, text), sizeof(((UmiChartAnnotationSnapshot *)0)->text), 0 }
};
static int ContractSnapshotEqual(const UmiChartAnnotationSnapshot *left, const UmiChartAnnotationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->pane_id, right->pane_id, sizeof(left->pane_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->time1 == right->time1 &&
        left->time2 == right->time2 &&
        left->value1 == right->value1 &&
        left->value2 == right->value2 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        left->locked == right->locked &&
        left->visible == right->visible &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiChartAnnotationSnapshot *item)
{
    item->pane_id[0] = 'v';
    item->kind[0] = 'v';
    item->time1 = (int64_t)6U;
    item->time2 = (int64_t)7U;
    item->value1 = (double)8U;
    item->value2 = (double)9U;
    item->text[0] = 'v';
    item->locked = (int)11U;
    item->visible = (int)12U;
}
#include "snapshot_contract_cases.h"
