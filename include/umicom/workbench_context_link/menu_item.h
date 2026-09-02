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

/**
 * Represent the workbench context link menu item data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context link menu item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_menu_item_init(UmiWorkbenchContextLinkMenuItem *record,
                                           const char *identity);
/**
 * Check that workbench context link menu item satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_menu_item_validate(
    const UmiWorkbenchContextLinkMenuItem *record);
/**
 * Copy workbench context link menu item into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_menu_item_copy(
    UmiWorkbenchContextLinkMenuItem *destination,
    const UmiWorkbenchContextLinkMenuItem *source);
/**
 * Provide the workbench context link menu item hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_menu_item_hash(
    const UmiWorkbenchContextLinkMenuItem *record);
/**
 * Provide the workbench context link menu item set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_menu_item_set_primary(
    UmiWorkbenchContextLinkMenuItem *record,
    const char *value);
/**
 * Provide the workbench context link menu item set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_menu_item_set_secondary(
    UmiWorkbenchContextLinkMenuItem *record,
    const char *value);
/**
 * Provide the workbench context link menu item touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_menu_item_touch(
    UmiWorkbenchContextLinkMenuItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
