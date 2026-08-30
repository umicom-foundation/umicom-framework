/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/command.h
 *
 * PURPOSE:
 *   Define the reusable context-link command record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkCommand {
    uint32_t structure_size;
    char command_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkCommand;

void umi_workbench_context_link_command_init(UmiWorkbenchContextLinkCommand *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_command_validate(
    const UmiWorkbenchContextLinkCommand *record);
UmiStatus umi_workbench_context_link_command_copy(
    UmiWorkbenchContextLinkCommand *destination,
    const UmiWorkbenchContextLinkCommand *source);
uint64_t umi_workbench_context_link_command_hash(
    const UmiWorkbenchContextLinkCommand *record);
UmiStatus umi_workbench_context_link_command_set_primary(
    UmiWorkbenchContextLinkCommand *record,
    const char *value);
UmiStatus umi_workbench_context_link_command_set_secondary(
    UmiWorkbenchContextLinkCommand *record,
    const char *value);
void umi_workbench_context_link_command_touch(
    UmiWorkbenchContextLinkCommand *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
