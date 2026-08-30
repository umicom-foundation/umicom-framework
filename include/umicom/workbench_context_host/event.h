/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/event.h
 *
 * PURPOSE:
 *   Define immutable context-host lifecycle and delivery events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_EVENT_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostEvent {
    uint32_t structure_size;
    char event_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostEventKind kind;
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char message[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t timestamp_ms;
} UmiWorkbenchContextHostEvent;
void umi_workbench_context_host_event_init(
    UmiWorkbenchContextHostEvent *event,UmiWorkbenchContextHostEventKind kind);
#ifdef __cplusplus
}
#endif
#endif
