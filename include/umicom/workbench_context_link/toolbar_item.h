/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/toolbar_item.h
 *
 * PURPOSE:
 *   Define the reusable context-link toolbar item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_TOOLBAR_ITEM_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_TOOLBAR_ITEM_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link toolbar item data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context link toolbar item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_toolbar_item_init(UmiWorkbenchContextLinkToolbarItem *record,
                                           const char *identity);
/**
 * Check that workbench context link toolbar item satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_toolbar_item_validate(
    const UmiWorkbenchContextLinkToolbarItem *record);
/**
 * Copy workbench context link toolbar item into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_toolbar_item_copy(
    UmiWorkbenchContextLinkToolbarItem *destination,
    const UmiWorkbenchContextLinkToolbarItem *source);
/**
 * Provide the workbench context link toolbar item hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_toolbar_item_hash(
    const UmiWorkbenchContextLinkToolbarItem *record);
/**
 * Provide the workbench context link toolbar item set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_toolbar_item_set_primary(
    UmiWorkbenchContextLinkToolbarItem *record,
    const char *value);
/**
 * Provide the workbench context link toolbar item set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_toolbar_item_set_secondary(
    UmiWorkbenchContextLinkToolbarItem *record,
    const char *value);
/**
 * Provide the workbench context link toolbar item touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_toolbar_item_touch(
    UmiWorkbenchContextLinkToolbarItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
