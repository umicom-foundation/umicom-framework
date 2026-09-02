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

/**
 * Represent the workbench context link command data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context link command from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_command_init(UmiWorkbenchContextLinkCommand *record,
                                           const char *identity);
/**
 * Check that workbench context link command satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_command_validate(
    const UmiWorkbenchContextLinkCommand *record);
/**
 * Copy workbench context link command into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_command_copy(
    UmiWorkbenchContextLinkCommand *destination,
    const UmiWorkbenchContextLinkCommand *source);
/**
 * Provide the workbench context link command hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_command_hash(
    const UmiWorkbenchContextLinkCommand *record);
/**
 * Provide the workbench context link command set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_command_set_primary(
    UmiWorkbenchContextLinkCommand *record,
    const char *value);
/**
 * Provide the workbench context link command set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_command_set_secondary(
    UmiWorkbenchContextLinkCommand *record,
    const char *value);
/**
 * Provide the workbench context link command touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_command_touch(
    UmiWorkbenchContextLinkCommand *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
