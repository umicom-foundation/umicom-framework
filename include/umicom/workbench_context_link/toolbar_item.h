/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/toolbar_item.h
 *
 * PURPOSE:
 *   Define the reusable context-link toolbar item contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_TOOLBAR_ITEM_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_TOOLBAR_ITEM_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkToolbarItem {
    uint32_t structure_size;
    char item_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkToolbarItem;

void umi_workbench_context_link_toolbar_item_init(UmiWorkbenchContextLinkToolbarItem *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_toolbar_item_validate(
    const UmiWorkbenchContextLinkToolbarItem *record);
UmiStatus umi_workbench_context_link_toolbar_item_copy(
    UmiWorkbenchContextLinkToolbarItem *destination,
    const UmiWorkbenchContextLinkToolbarItem *source);
uint64_t umi_workbench_context_link_toolbar_item_hash(
    const UmiWorkbenchContextLinkToolbarItem *record);
UmiStatus umi_workbench_context_link_toolbar_item_set_primary(
    UmiWorkbenchContextLinkToolbarItem *record,
    const char *value);
UmiStatus umi_workbench_context_link_toolbar_item_set_secondary(
    UmiWorkbenchContextLinkToolbarItem *record,
    const char *value);
void umi_workbench_context_link_toolbar_item_touch(
    UmiWorkbenchContextLinkToolbarItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
