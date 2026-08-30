/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/route_record.h
 *
 * PURPOSE:
 *   Define the reusable source routing record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_ROUTE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_ROUTE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceRouteRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceRouteRecord;

void umi_workbench_context_source_route_record_init(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_route_record_validate(
    const UmiWorkbenchContextSourceRouteRecord *record);
UmiStatus umi_workbench_context_source_route_record_set_source(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_route_record_set_panel(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_route_record_set_subject(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_route_record_set_group(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_route_record_set_label(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_route_record_hash(
    const UmiWorkbenchContextSourceRouteRecord *record);
void umi_workbench_context_source_route_record_touch(
    UmiWorkbenchContextSourceRouteRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
