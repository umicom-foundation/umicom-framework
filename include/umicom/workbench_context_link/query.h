/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/query.h
 *
 * PURPOSE:
 *   Define the reusable context-link query record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_QUERY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkQuery {
    uint32_t structure_size;
    char query_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkQuery;

/**
 * Initialise workbench context link query from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_link_query_init(UmiWorkbenchContextLinkQuery *record,
                                           const char *identity);
/**
 * Check that workbench context link query satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_query_validate(
    const UmiWorkbenchContextLinkQuery *record);
/**
 * Copy workbench context link query into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_context_link_query_copy(
    UmiWorkbenchContextLinkQuery *destination,
    const UmiWorkbenchContextLinkQuery *source);
/**
 * Provide the workbench context link query hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_query_hash(
    const UmiWorkbenchContextLinkQuery *record);
/**
 * Provide the workbench context link query set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_query_set_primary(
    UmiWorkbenchContextLinkQuery *record,
    const char *value);
/**
 * Provide the workbench context link query set secondary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_query_set_secondary(
    UmiWorkbenchContextLinkQuery *record,
    const char *value);
/**
 * Provide the workbench context link query touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_query_touch(
    UmiWorkbenchContextLinkQuery *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
