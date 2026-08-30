/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/delivery_target.h
 *
 * PURPOSE:
 *   Define the reusable context delivery target contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_TARGET_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_TARGET_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkDeliveryTarget {
    uint32_t structure_size;
    char target_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkDeliveryTarget;

void umi_workbench_context_link_delivery_target_init(UmiWorkbenchContextLinkDeliveryTarget *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_delivery_target_validate(
    const UmiWorkbenchContextLinkDeliveryTarget *record);
UmiStatus umi_workbench_context_link_delivery_target_copy(
    UmiWorkbenchContextLinkDeliveryTarget *destination,
    const UmiWorkbenchContextLinkDeliveryTarget *source);
uint64_t umi_workbench_context_link_delivery_target_hash(
    const UmiWorkbenchContextLinkDeliveryTarget *record);
UmiStatus umi_workbench_context_link_delivery_target_set_primary(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    const char *value);
UmiStatus umi_workbench_context_link_delivery_target_set_secondary(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    const char *value);
void umi_workbench_context_link_delivery_target_touch(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
