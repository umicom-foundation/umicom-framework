/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_entry.h
 *
 * PURPOSE:
 *   Define the reusable context navigation history entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_ENTRY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link history entry data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context link history entry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_history_entry_init(UmiWorkbenchContextLinkHistoryEntry *record,
                                           const char *identity);
/**
 * Check that workbench context link history entry satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_history_entry_validate(
    const UmiWorkbenchContextLinkHistoryEntry *record);
/**
 * Copy workbench context link history entry into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_history_entry_copy(
    UmiWorkbenchContextLinkHistoryEntry *destination,
    const UmiWorkbenchContextLinkHistoryEntry *source);
/**
 * Provide the workbench context link history entry hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_history_entry_hash(
    const UmiWorkbenchContextLinkHistoryEntry *record);
/**
 * Provide the workbench context link history entry set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_history_entry_set_primary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
/**
 * Provide the workbench context link history entry set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_history_entry_set_secondary(
    UmiWorkbenchContextLinkHistoryEntry *record,
    const char *value);
/**
 * Provide the workbench context link history entry touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_history_entry_touch(
    UmiWorkbenchContextLinkHistoryEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
