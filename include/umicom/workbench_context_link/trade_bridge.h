/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/trade_bridge.h
 *
 * PURPOSE:
 *   Define the reusable trade context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_TRADE_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_TRADE_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkTradeBridge {
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
} UmiWorkbenchContextLinkTradeBridge;

void umi_workbench_context_link_trade_bridge_init(UmiWorkbenchContextLinkTradeBridge *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_trade_bridge_validate(
    const UmiWorkbenchContextLinkTradeBridge *record);
UmiStatus umi_workbench_context_link_trade_bridge_copy(
    UmiWorkbenchContextLinkTradeBridge *destination,
    const UmiWorkbenchContextLinkTradeBridge *source);
uint64_t umi_workbench_context_link_trade_bridge_hash(
    const UmiWorkbenchContextLinkTradeBridge *record);
UmiStatus umi_workbench_context_link_trade_bridge_set_primary(
    UmiWorkbenchContextLinkTradeBridge *record,
    const char *value);
UmiStatus umi_workbench_context_link_trade_bridge_set_secondary(
    UmiWorkbenchContextLinkTradeBridge *record,
    const char *value);
void umi_workbench_context_link_trade_bridge_touch(
    UmiWorkbenchContextLinkTradeBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
