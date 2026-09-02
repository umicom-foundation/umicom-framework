/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/source_location_bridge.h
 *
 * PURPOSE:
 *   Define the reusable source-location context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SOURCE_LOCATION_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SOURCE_LOCATION_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link source location bridge data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextLinkSourceLocationBridge {
    uint32_t structure_size;
    char bridge_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkSourceLocationBridge;

/**
 * Initialise workbench context link source location bridge from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_source_location_bridge_init(UmiWorkbenchContextLinkSourceLocationBridge *record,
                                           const char *identity);
/**
 * Check that workbench context link source location bridge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_source_location_bridge_validate(
    const UmiWorkbenchContextLinkSourceLocationBridge *record);
/**
 * Copy workbench context link source location bridge into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_workbench_context_link_source_location_bridge_copy(
    UmiWorkbenchContextLinkSourceLocationBridge *destination,
    const UmiWorkbenchContextLinkSourceLocationBridge *source);
/**
 * Provide the workbench context link source location bridge hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_link_source_location_bridge_hash(
    const UmiWorkbenchContextLinkSourceLocationBridge *record);
/**
 * Provide the workbench context link source location bridge set primary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_source_location_bridge_set_primary(
    UmiWorkbenchContextLinkSourceLocationBridge *record,
    const char *value);
/**
 * Provide the workbench context link source location bridge set secondary operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_context_link_source_location_bridge_set_secondary(
    UmiWorkbenchContextLinkSourceLocationBridge *record,
    const char *value);
/**
 * Provide the workbench context link source location bridge touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_source_location_bridge_touch(
    UmiWorkbenchContextLinkSourceLocationBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
