/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/delivery_state.h
 *
 * PURPOSE:
 *   Define the reusable event delivery state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DELIVERY_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DELIVERY_STATE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventDeliveryState {
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
} UmiWorkbenchContextEventDeliveryState;

void umi_workbench_context_event_delivery_state_init(
    UmiWorkbenchContextEventDeliveryState *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_delivery_state_validate(
    const UmiWorkbenchContextEventDeliveryState *record);
UmiStatus umi_workbench_context_event_delivery_state_set_source(
    UmiWorkbenchContextEventDeliveryState *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_delivery_state_set_subject(
    UmiWorkbenchContextEventDeliveryState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_delivery_state_set_group(
    UmiWorkbenchContextEventDeliveryState *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_delivery_state_set_label(
    UmiWorkbenchContextEventDeliveryState *record,
    const char *label);
uint64_t umi_workbench_context_event_delivery_state_hash(
    const UmiWorkbenchContextEventDeliveryState *record);
void umi_workbench_context_event_delivery_state_touch(
    UmiWorkbenchContextEventDeliveryState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
