/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/debug_record.h
 *
 * PURPOSE:
 *   Define the reusable debug event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DEBUG_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DEBUG_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventDebugRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventDebugRecord;

void umi_workbench_context_event_debug_record_init(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_debug_record_validate(
    const UmiWorkbenchContextEventDebugRecord *record);
UmiStatus umi_workbench_context_event_debug_record_set_source(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_debug_record_set_subject(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_debug_record_set_group(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_debug_record_set_label(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_debug_record_hash(
    const UmiWorkbenchContextEventDebugRecord *record);
void umi_workbench_context_event_debug_record_touch(
    UmiWorkbenchContextEventDebugRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
