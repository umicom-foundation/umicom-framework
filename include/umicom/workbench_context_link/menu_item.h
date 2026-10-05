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
#include "umicom/base/value_archive.h"

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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_menu_item_archive_encode(const UmiWorkbenchContextLinkMenuItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_menu_item_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkMenuItem *value);

#ifdef __cplusplus
}
#endif

#endif
