/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/instrument_record.h
 *
 * PURPOSE:
 *   Define the reusable instrument event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventInstrumentRecord {
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
} UmiWorkbenchContextEventInstrumentRecord;

void umi_workbench_context_event_instrument_record_init(
    UmiWorkbenchContextEventInstrumentRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_instrument_record_validate(
    const UmiWorkbenchContextEventInstrumentRecord *record);
UmiStatus umi_workbench_context_event_instrument_record_set_source(
    UmiWorkbenchContextEventInstrumentRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_instrument_record_set_subject(
    UmiWorkbenchContextEventInstrumentRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_instrument_record_set_group(
    UmiWorkbenchContextEventInstrumentRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_instrument_record_set_label(
    UmiWorkbenchContextEventInstrumentRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_instrument_record_hash(
    const UmiWorkbenchContextEventInstrumentRecord *record);
void umi_workbench_context_event_instrument_record_touch(
    UmiWorkbenchContextEventInstrumentRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
