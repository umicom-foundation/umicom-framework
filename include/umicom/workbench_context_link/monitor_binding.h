/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/monitor_binding.h
 *
 * PURPOSE:
 *   Define the reusable monitor context binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_MONITOR_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_MONITOR_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkMonitorBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkMonitorBinding;

void umi_workbench_context_link_monitor_binding_init(UmiWorkbenchContextLinkMonitorBinding *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_monitor_binding_validate(
    const UmiWorkbenchContextLinkMonitorBinding *record);
UmiStatus umi_workbench_context_link_monitor_binding_copy(
    UmiWorkbenchContextLinkMonitorBinding *destination,
    const UmiWorkbenchContextLinkMonitorBinding *source);
uint64_t umi_workbench_context_link_monitor_binding_hash(
    const UmiWorkbenchContextLinkMonitorBinding *record);
UmiStatus umi_workbench_context_link_monitor_binding_set_primary(
    UmiWorkbenchContextLinkMonitorBinding *record,
    const char *value);
UmiStatus umi_workbench_context_link_monitor_binding_set_secondary(
    UmiWorkbenchContextLinkMonitorBinding *record,
    const char *value);
void umi_workbench_context_link_monitor_binding_touch(
    UmiWorkbenchContextLinkMonitorBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
