/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/command_history.h
 *
 * PURPOSE:
 *   Define the reusable context-link command history record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_HISTORY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_COMMAND_HISTORY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link command history data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context link command history from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_command_history_init(UmiWorkbenchContextLinkCommandHistory *record,
                                           const char *identity);
/**
 * Check that workbench context link command history satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_command_history_validate(
    const UmiWorkbenchContextLinkCommandHistory *record);
/**
 * Copy workbench context link command history into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_command_history_copy(
    UmiWorkbenchContextLinkCommandHistory *destination,
    const UmiWorkbenchContextLinkCommandHistory *source);
/**
 * Provide the workbench context link command history hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_command_history_hash(
    const UmiWorkbenchContextLinkCommandHistory *record);
/**
 * Provide the workbench context link command history set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_command_history_set_primary(
    UmiWorkbenchContextLinkCommandHistory *record,
    const char *value);
/**
 * Provide the workbench context link command history set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_command_history_set_secondary(
    UmiWorkbenchContextLinkCommandHistory *record,
    const char *value);
/**
 * Provide the workbench context link command history touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_command_history_touch(
    UmiWorkbenchContextLinkCommandHistory *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
