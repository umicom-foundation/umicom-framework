/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/workspace_record.h
 *
 * PURPOSE:
 *   Define the reusable workspace event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventWorkspaceRecord {
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
} UmiWorkbenchContextEventWorkspaceRecord;

void umi_workbench_context_event_workspace_record_init(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_workspace_record_validate(
    const UmiWorkbenchContextEventWorkspaceRecord *record);
UmiStatus umi_workbench_context_event_workspace_record_set_source(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_workspace_record_set_subject(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_workspace_record_set_group(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_workspace_record_set_label(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_workspace_record_hash(
    const UmiWorkbenchContextEventWorkspaceRecord *record);
void umi_workbench_context_event_workspace_record_touch(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
