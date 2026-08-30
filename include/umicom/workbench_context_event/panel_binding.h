/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/panel_binding.h
 *
 * PURPOSE:
 *   Define the reusable panel event binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PANEL_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PANEL_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventPanelBinding {
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
} UmiWorkbenchContextEventPanelBinding;

void umi_workbench_context_event_panel_binding_init(
    UmiWorkbenchContextEventPanelBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_panel_binding_validate(
    const UmiWorkbenchContextEventPanelBinding *record);
UmiStatus umi_workbench_context_event_panel_binding_set_source(
    UmiWorkbenchContextEventPanelBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_panel_binding_set_subject(
    UmiWorkbenchContextEventPanelBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_panel_binding_set_group(
    UmiWorkbenchContextEventPanelBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_panel_binding_set_label(
    UmiWorkbenchContextEventPanelBinding *record,
    const char *label);
uint64_t umi_workbench_context_event_panel_binding_hash(
    const UmiWorkbenchContextEventPanelBinding *record);
void umi_workbench_context_event_panel_binding_touch(
    UmiWorkbenchContextEventPanelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
