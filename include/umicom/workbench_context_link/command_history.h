/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/command_history.h
 *
 * PURPOSE:
 *   Define the reusable context-link command history record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_HISTORY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_HISTORY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkCommandHistory {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkCommandHistory;

void umi_workbench_context_link_command_history_init(UmiWorkbenchContextLinkCommandHistory *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_command_history_validate(
    const UmiWorkbenchContextLinkCommandHistory *record);
UmiStatus umi_workbench_context_link_command_history_copy(
    UmiWorkbenchContextLinkCommandHistory *destination,
    const UmiWorkbenchContextLinkCommandHistory *source);
uint64_t umi_workbench_context_link_command_history_hash(
    const UmiWorkbenchContextLinkCommandHistory *record);
UmiStatus umi_workbench_context_link_command_history_set_primary(
    UmiWorkbenchContextLinkCommandHistory *record,
    const char *value);
UmiStatus umi_workbench_context_link_command_history_set_secondary(
    UmiWorkbenchContextLinkCommandHistory *record,
    const char *value);
void umi_workbench_context_link_command_history_touch(
    UmiWorkbenchContextLinkCommandHistory *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
