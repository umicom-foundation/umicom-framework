/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/delivery_state.h
 *
 * PURPOSE:
 *   Define the reusable delivery runtime state record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_delivery_state_init(UmiWorkbenchContextLinkDeliveryState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_delivery_state_validate(
    const UmiWorkbenchContextLinkDeliveryState *record);
UmiStatus umi_workbench_context_link_delivery_state_copy(
    UmiWorkbenchContextLinkDeliveryState *destination,
    const UmiWorkbenchContextLinkDeliveryState *source);
uint64_t umi_workbench_context_link_delivery_state_hash(
    const UmiWorkbenchContextLinkDeliveryState *record);
UmiStatus umi_workbench_context_link_delivery_state_set_primary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value);
UmiStatus umi_workbench_context_link_delivery_state_set_secondary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value);
void umi_workbench_context_link_delivery_state_touch(
    UmiWorkbenchContextLinkDeliveryState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
