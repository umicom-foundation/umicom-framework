/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/layout_binding.h
 *
 * PURPOSE:
 *   Define the reusable layout context binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkLayoutBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkLayoutBinding;

void umi_workbench_context_link_layout_binding_init(UmiWorkbenchContextLinkLayoutBinding *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_layout_binding_validate(
    const UmiWorkbenchContextLinkLayoutBinding *record);
UmiStatus umi_workbench_context_link_layout_binding_copy(
    UmiWorkbenchContextLinkLayoutBinding *destination,
    const UmiWorkbenchContextLinkLayoutBinding *source);
uint64_t umi_workbench_context_link_layout_binding_hash(
    const UmiWorkbenchContextLinkLayoutBinding *record);
UmiStatus umi_workbench_context_link_layout_binding_set_primary(
    UmiWorkbenchContextLinkLayoutBinding *record,
    const char *value);
UmiStatus umi_workbench_context_link_layout_binding_set_secondary(
    UmiWorkbenchContextLinkLayoutBinding *record,
    const char *value);
void umi_workbench_context_link_layout_binding_touch(
    UmiWorkbenchContextLinkLayoutBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
