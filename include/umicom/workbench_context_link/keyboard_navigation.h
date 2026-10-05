/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/keyboard_navigation.h
 *
 * PURPOSE:
 *   Define the reusable context-link keyboard navigation state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_KEYBOARD_NAVIGATION_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_KEYBOARD_NAVIGATION_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link keyboard navigation data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextLinkKeyboardNavigation {
    uint32_t structure_size;
    char navigation_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkKeyboardNavigation;

/**
 * Initialise workbench context link keyboard navigation from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_keyboard_navigation_init(UmiWorkbenchContextLinkKeyboardNavigation *record,
                                           const char *identity);
/**
 * Check that workbench context link keyboard navigation satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_keyboard_navigation_validate(
    const UmiWorkbenchContextLinkKeyboardNavigation *record);
/**
 * Copy workbench context link keyboard navigation into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_workbench_context_link_keyboard_navigation_copy(
    UmiWorkbenchContextLinkKeyboardNavigation *destination,
    const UmiWorkbenchContextLinkKeyboardNavigation *source);
/**
 * Provide the workbench context link keyboard navigation hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_link_keyboard_navigation_hash(
    const UmiWorkbenchContextLinkKeyboardNavigation *record);
/**
 * Provide the workbench context link keyboard navigation set primary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_keyboard_navigation_set_primary(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    const char *value);
/**
 * Provide the workbench context link keyboard navigation set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_keyboard_navigation_set_secondary(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    const char *value);
/**
 * Provide the workbench context link keyboard navigation touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_keyboard_navigation_touch(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_keyboard_navigation_archive_encode(const UmiWorkbenchContextLinkKeyboardNavigation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_keyboard_navigation_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkKeyboardNavigation *value);

#ifdef __cplusplus
}
#endif

#endif
