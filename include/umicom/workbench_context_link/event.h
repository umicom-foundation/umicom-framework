/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/event.h
 *
 * PURPOSE:
 *   Define the reusable context-link event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_EVENT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkEvent {
    uint32_t structure_size;
    char event_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkEvent;

void umi_workbench_context_link_event_init(UmiWorkbenchContextLinkEvent *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_event_validate(
    const UmiWorkbenchContextLinkEvent *record);
UmiStatus umi_workbench_context_link_event_copy(
    UmiWorkbenchContextLinkEvent *destination,
    const UmiWorkbenchContextLinkEvent *source);
uint64_t umi_workbench_context_link_event_hash(
    const UmiWorkbenchContextLinkEvent *record);
UmiStatus umi_workbench_context_link_event_set_primary(
    UmiWorkbenchContextLinkEvent *record,
    const char *value);
UmiStatus umi_workbench_context_link_event_set_secondary(
    UmiWorkbenchContextLinkEvent *record,
    const char *value);
void umi_workbench_context_link_event_touch(
    UmiWorkbenchContextLinkEvent *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
