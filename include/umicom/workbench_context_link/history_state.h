/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_state.h
 *
 * PURPOSE:
 *   Define the reusable history runtime state record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkHistoryState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char cursor_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkHistoryState;

void umi_workbench_context_link_history_state_init(UmiWorkbenchContextLinkHistoryState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_history_state_validate(
    const UmiWorkbenchContextLinkHistoryState *record);
UmiStatus umi_workbench_context_link_history_state_copy(
    UmiWorkbenchContextLinkHistoryState *destination,
    const UmiWorkbenchContextLinkHistoryState *source);
uint64_t umi_workbench_context_link_history_state_hash(
    const UmiWorkbenchContextLinkHistoryState *record);
UmiStatus umi_workbench_context_link_history_state_set_primary(
    UmiWorkbenchContextLinkHistoryState *record,
    const char *value);
UmiStatus umi_workbench_context_link_history_state_set_secondary(
    UmiWorkbenchContextLinkHistoryState *record,
    const char *value);
void umi_workbench_context_link_history_state_touch(
    UmiWorkbenchContextLinkHistoryState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
