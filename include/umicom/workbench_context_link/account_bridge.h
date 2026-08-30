/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/account_bridge.h
 *
 * PURPOSE:
 *   Define the reusable account context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACCOUNT_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACCOUNT_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkAccountBridge {
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
} UmiWorkbenchContextLinkAccountBridge;

void umi_workbench_context_link_account_bridge_init(UmiWorkbenchContextLinkAccountBridge *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_account_bridge_validate(
    const UmiWorkbenchContextLinkAccountBridge *record);
UmiStatus umi_workbench_context_link_account_bridge_copy(
    UmiWorkbenchContextLinkAccountBridge *destination,
    const UmiWorkbenchContextLinkAccountBridge *source);
uint64_t umi_workbench_context_link_account_bridge_hash(
    const UmiWorkbenchContextLinkAccountBridge *record);
UmiStatus umi_workbench_context_link_account_bridge_set_primary(
    UmiWorkbenchContextLinkAccountBridge *record,
    const char *value);
UmiStatus umi_workbench_context_link_account_bridge_set_secondary(
    UmiWorkbenchContextLinkAccountBridge *record,
    const char *value);
void umi_workbench_context_link_account_bridge_touch(
    UmiWorkbenchContextLinkAccountBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
