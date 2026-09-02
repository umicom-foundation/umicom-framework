/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/context_diff.h
 *
 * PURPOSE:
 *   Define the reusable context comparison record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_DIFF_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_DIFF_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link context diff data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkContextDiff {
    uint32_t structure_size;
    char diff_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char left_context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char right_context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkContextDiff;

/**
 * Initialise workbench context link context diff from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_context_diff_init(UmiWorkbenchContextLinkContextDiff *record,
                                           const char *identity);
/**
 * Check that workbench context link context diff satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_context_diff_validate(
    const UmiWorkbenchContextLinkContextDiff *record);
/**
 * Copy workbench context link context diff into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_context_diff_copy(
    UmiWorkbenchContextLinkContextDiff *destination,
    const UmiWorkbenchContextLinkContextDiff *source);
/**
 * Provide the workbench context link context diff hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_context_diff_hash(
    const UmiWorkbenchContextLinkContextDiff *record);
/**
 * Provide the workbench context link context diff set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_context_diff_set_primary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value);
/**
 * Provide the workbench context link context diff set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_context_diff_set_secondary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value);
/**
 * Provide the workbench context link context diff touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_context_diff_touch(
    UmiWorkbenchContextLinkContextDiff *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
