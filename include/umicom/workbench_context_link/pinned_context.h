/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/pinned_context.h
 *
 * PURPOSE:
 *   Define the reusable pinned context record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PINNED_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PINNED_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link pinned context data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkPinnedContext {
    uint32_t structure_size;
    char pin_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkPinnedContext;

/**
 * Initialise workbench context link pinned context from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_pinned_context_init(UmiWorkbenchContextLinkPinnedContext *record,
                                           const char *identity);
/**
 * Check that workbench context link pinned context satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_pinned_context_validate(
    const UmiWorkbenchContextLinkPinnedContext *record);
/**
 * Copy workbench context link pinned context into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_pinned_context_copy(
    UmiWorkbenchContextLinkPinnedContext *destination,
    const UmiWorkbenchContextLinkPinnedContext *source);
/**
 * Provide the workbench context link pinned context hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_pinned_context_hash(
    const UmiWorkbenchContextLinkPinnedContext *record);
/**
 * Provide the workbench context link pinned context set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_pinned_context_set_primary(
    UmiWorkbenchContextLinkPinnedContext *record,
    const char *value);
/**
 * Provide the workbench context link pinned context set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_pinned_context_set_secondary(
    UmiWorkbenchContextLinkPinnedContext *record,
    const char *value);
/**
 * Provide the workbench context link pinned context touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_pinned_context_touch(
    UmiWorkbenchContextLinkPinnedContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
