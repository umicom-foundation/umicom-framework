/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/event_record.h
 *
 * PURPOSE:
 *   Define the reusable normalised event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceEventRecord {
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
} UmiWorkbenchContextSourceEventRecord;

void umi_workbench_context_source_event_record_init(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_event_record_validate(
    const UmiWorkbenchContextSourceEventRecord *record);
UmiStatus umi_workbench_context_source_event_record_set_source(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_event_record_set_panel(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_event_record_set_subject(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_event_record_set_group(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_event_record_set_label(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_event_record_hash(
    const UmiWorkbenchContextSourceEventRecord *record);
void umi_workbench_context_source_event_record_touch(
    UmiWorkbenchContextSourceEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
