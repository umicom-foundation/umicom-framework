/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_entry.h
 *
 * PURPOSE:
 *   Define the reusable context navigation history entry contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkHistoryEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkHistoryEntry;

void umi_workbench_context_link_history_entry_init(UmiWorkbenchContextLinkHistoryEntry *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_history_entry_validate(
    const UmiWorkbenchContextLinkHistoryEntry *record);
UmiStatus umi_workbench_context_link_history_entry_copy(
    UmiWorkbenchContextLinkHistoryEntry *destination,
    const UmiWorkbenchContextLinkHistoryEntry *source);
uint64_t umi_workbench_context_link_history_entry_hash(
    const UmiWorkbenchContextLinkHistoryEntry *record);
UmiStatus umi_workbench_context_link_history_entry_set_primary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
UmiStatus umi_workbench_context_link_history_entry_set_secondary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
void umi_workbench_context_link_history_entry_touch(
    UmiWorkbenchContextLinkHistoryEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
