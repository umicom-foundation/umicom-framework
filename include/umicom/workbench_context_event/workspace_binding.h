/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace event binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventWorkspaceBinding {
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
} UmiWorkbenchContextEventWorkspaceBinding;

void umi_workbench_context_event_workspace_binding_init(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_workspace_binding_validate(
    const UmiWorkbenchContextEventWorkspaceBinding *record);
UmiStatus umi_workbench_context_event_workspace_binding_set_source(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_workspace_binding_set_subject(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_workspace_binding_set_group(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_workspace_binding_set_label(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    const char *label);
uint64_t umi_workbench_context_event_workspace_binding_hash(
    const UmiWorkbenchContextEventWorkspaceBinding *record);
void umi_workbench_context_event_workspace_binding_touch(
    UmiWorkbenchContextEventWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
