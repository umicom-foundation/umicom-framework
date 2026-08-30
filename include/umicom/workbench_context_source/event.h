/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/event.h
 *
 * PURPOSE:
 *   Define the reusable source lifecycle event contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceEvent {
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
} UmiWorkbenchContextSourceEvent;

void umi_workbench_context_source_event_init(
    UmiWorkbenchContextSourceEvent *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_event_validate(
    const UmiWorkbenchContextSourceEvent *record);
UmiStatus umi_workbench_context_source_event_set_source(
    UmiWorkbenchContextSourceEvent *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_event_set_panel(
    UmiWorkbenchContextSourceEvent *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_event_set_subject(
    UmiWorkbenchContextSourceEvent *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_event_set_group(
    UmiWorkbenchContextSourceEvent *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_event_set_label(
    UmiWorkbenchContextSourceEvent *record,
    const char *label);
uint64_t umi_workbench_context_source_event_hash(
    const UmiWorkbenchContextSourceEvent *record);
void umi_workbench_context_source_event_touch(
    UmiWorkbenchContextSourceEvent *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
