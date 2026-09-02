/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/active_context.h
 *
 * PURPOSE:
 *   Define the reusable active group context contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link active context data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkActiveContext {
    uint32_t structure_size;
    char active_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkActiveContext;

/**
 * Initialise workbench context link active context from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_active_context_init(UmiWorkbenchContextLinkActiveContext *record,
                                           const char *identity);
/**
 * Check that workbench context link active context satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_active_context_validate(
    const UmiWorkbenchContextLinkActiveContext *record);
/**
 * Copy workbench context link active context into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_active_context_copy(
    UmiWorkbenchContextLinkActiveContext *destination,
    const UmiWorkbenchContextLinkActiveContext *source);
/**
 * Provide the workbench context link active context hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_active_context_hash(
    const UmiWorkbenchContextLinkActiveContext *record);
/**
 * Provide the workbench context link active context set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_active_context_set_primary(
    UmiWorkbenchContextLinkActiveContext *record,
    const char *value);
/**
 * Provide the workbench context link active context set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_active_context_set_secondary(
    UmiWorkbenchContextLinkActiveContext *record,
    const char *value);
/**
 * Provide the workbench context link active context touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_active_context_touch(
    UmiWorkbenchContextLinkActiveContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
