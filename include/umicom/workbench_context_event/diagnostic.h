/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/diagnostic.h
 *
 * PURPOSE:
 *   Define the reusable event diagnostic contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventDiagnostic {
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
} UmiWorkbenchContextEventDiagnostic;

void umi_workbench_context_event_diagnostic_init(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_diagnostic_validate(
    const UmiWorkbenchContextEventDiagnostic *record);
UmiStatus umi_workbench_context_event_diagnostic_set_source(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_diagnostic_set_subject(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_diagnostic_set_group(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_diagnostic_set_label(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *label);
uint64_t umi_workbench_context_event_diagnostic_hash(
    const UmiWorkbenchContextEventDiagnostic *record);
void umi_workbench_context_event_diagnostic_touch(
    UmiWorkbenchContextEventDiagnostic *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
