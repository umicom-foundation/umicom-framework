/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/media_bridge.h
 *
 * PURPOSE:
 *   Define the reusable media context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_MEDIA_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_MEDIA_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link media bridge data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkMediaBridge {
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
} UmiWorkbenchContextLinkMediaBridge;

/**
 * Initialise workbench context link media bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_media_bridge_init(UmiWorkbenchContextLinkMediaBridge *record,
                                           const char *identity);
/**
 * Check that workbench context link media bridge satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_media_bridge_validate(
    const UmiWorkbenchContextLinkMediaBridge *record);
/**
 * Copy workbench context link media bridge into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_media_bridge_copy(
    UmiWorkbenchContextLinkMediaBridge *destination,
    const UmiWorkbenchContextLinkMediaBridge *source);
/**
 * Provide the workbench context link media bridge hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_media_bridge_hash(
    const UmiWorkbenchContextLinkMediaBridge *record);
/**
 * Provide the workbench context link media bridge set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_media_bridge_set_primary(
    UmiWorkbenchContextLinkMediaBridge *record,
    const char *value);
/**
 * Provide the workbench context link media bridge set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_media_bridge_set_secondary(
    UmiWorkbenchContextLinkMediaBridge *record,
    const char *value);
/**
 * Provide the workbench context link media bridge touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_media_bridge_touch(
    UmiWorkbenchContextLinkMediaBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
