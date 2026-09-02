/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/delivery_state.h
 *
 * PURPOSE:
 *   Define the reusable delivery runtime state record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link delivery state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkDeliveryState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char delivery_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkDeliveryState;

/**
 * Initialise workbench context link delivery state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_delivery_state_init(UmiWorkbenchContextLinkDeliveryState *record,
                                           const char *identity);
/**
 * Check that workbench context link delivery state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_delivery_state_validate(
    const UmiWorkbenchContextLinkDeliveryState *record);
/**
 * Copy workbench context link delivery state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_delivery_state_copy(
    UmiWorkbenchContextLinkDeliveryState *destination,
    const UmiWorkbenchContextLinkDeliveryState *source);
/**
 * Provide the workbench context link delivery state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_delivery_state_hash(
    const UmiWorkbenchContextLinkDeliveryState *record);
/**
 * Provide the workbench context link delivery state set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_state_set_primary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value);
/**
 * Provide the workbench context link delivery state set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_state_set_secondary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value);
/**
 * Provide the workbench context link delivery state touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_delivery_state_touch(
    UmiWorkbenchContextLinkDeliveryState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
