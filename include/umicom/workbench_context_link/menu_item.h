/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/menu_item.h
 *
 * PURPOSE:
 *   Define the reusable context-link menu item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_MENU_ITEM_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_MENU_ITEM_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkMenuItem {
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
} UmiWorkbenchContextLinkMenuItem;

void umi_workbench_context_link_menu_item_init(UmiWorkbenchContextLinkMenuItem *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_menu_item_validate(
    const UmiWorkbenchContextLinkMenuItem *record);
UmiStatus umi_workbench_context_link_menu_item_copy(
    UmiWorkbenchContextLinkMenuItem *destination,
    const UmiWorkbenchContextLinkMenuItem *source);
uint64_t umi_workbench_context_link_menu_item_hash(
    const UmiWorkbenchContextLinkMenuItem *record);
UmiStatus umi_workbench_context_link_menu_item_set_primary(
    UmiWorkbenchContextLinkMenuItem *record,
    const char *value);
UmiStatus umi_workbench_context_link_menu_item_set_secondary(
    UmiWorkbenchContextLinkMenuItem *record,
    const char *value);
void umi_workbench_context_link_menu_item_touch(
    UmiWorkbenchContextLinkMenuItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
