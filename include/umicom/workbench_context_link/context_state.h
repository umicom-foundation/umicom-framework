/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/context_state.h
 *
 * PURPOSE:
 *   Define the reusable context runtime state record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkContextState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkContextState;

void umi_workbench_context_link_context_state_init(UmiWorkbenchContextLinkContextState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_context_state_validate(
    const UmiWorkbenchContextLinkContextState *record);
UmiStatus umi_workbench_context_link_context_state_copy(
    UmiWorkbenchContextLinkContextState *destination,
    const UmiWorkbenchContextLinkContextState *source);
uint64_t umi_workbench_context_link_context_state_hash(
    const UmiWorkbenchContextLinkContextState *record);
UmiStatus umi_workbench_context_link_context_state_set_primary(
    UmiWorkbenchContextLinkContextState *record,
    const char *value);
UmiStatus umi_workbench_context_link_context_state_set_secondary(
    UmiWorkbenchContextLinkContextState *record,
    const char *value);
void umi_workbench_context_link_context_state_touch(
    UmiWorkbenchContextLinkContextState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
