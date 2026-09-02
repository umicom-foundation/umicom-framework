/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/recent_context.h
 *
 * PURPOSE:
 *   Define the reusable recent context record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_RECENT_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_RECENT_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link recent context data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkRecentContext {
    uint32_t structure_size;
    char recent_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkRecentContext;

/**
 * Initialise workbench context link recent context from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_recent_context_init(UmiWorkbenchContextLinkRecentContext *record,
                                           const char *identity);
/**
 * Check that workbench context link recent context satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_recent_context_validate(
    const UmiWorkbenchContextLinkRecentContext *record);
/**
 * Copy workbench context link recent context into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_recent_context_copy(
    UmiWorkbenchContextLinkRecentContext *destination,
    const UmiWorkbenchContextLinkRecentContext *source);
/**
 * Provide the workbench context link recent context hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_recent_context_hash(
    const UmiWorkbenchContextLinkRecentContext *record);
/**
 * Provide the workbench context link recent context set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_recent_context_set_primary(
    UmiWorkbenchContextLinkRecentContext *record,
    const char *value);
/**
 * Provide the workbench context link recent context set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_recent_context_set_secondary(
    UmiWorkbenchContextLinkRecentContext *record,
    const char *value);
/**
 * Provide the workbench context link recent context touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_recent_context_touch(
    UmiWorkbenchContextLinkRecentContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
