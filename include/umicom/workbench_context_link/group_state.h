/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_state.h
 *
 * PURPOSE:
 *   Define the reusable group runtime state record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkGroupState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkGroupState;

void umi_workbench_context_link_group_state_init(UmiWorkbenchContextLinkGroupState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_group_state_validate(
    const UmiWorkbenchContextLinkGroupState *record);
UmiStatus umi_workbench_context_link_group_state_copy(
    UmiWorkbenchContextLinkGroupState *destination,
    const UmiWorkbenchContextLinkGroupState *source);
uint64_t umi_workbench_context_link_group_state_hash(
    const UmiWorkbenchContextLinkGroupState *record);
UmiStatus umi_workbench_context_link_group_state_set_primary(
    UmiWorkbenchContextLinkGroupState *record,
    const char *value);
UmiStatus umi_workbench_context_link_group_state_set_secondary(
    UmiWorkbenchContextLinkGroupState *record,
    const char *value);
void umi_workbench_context_link_group_state_touch(
    UmiWorkbenchContextLinkGroupState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
