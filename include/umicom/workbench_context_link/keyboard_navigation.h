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

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_keyboard_navigation_init(UmiWorkbenchContextLinkKeyboardNavigation *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_keyboard_navigation_validate(
    const UmiWorkbenchContextLinkKeyboardNavigation *record);
UmiStatus umi_workbench_context_link_keyboard_navigation_copy(
    UmiWorkbenchContextLinkKeyboardNavigation *destination,
    const UmiWorkbenchContextLinkKeyboardNavigation *source);
uint64_t umi_workbench_context_link_keyboard_navigation_hash(
    const UmiWorkbenchContextLinkKeyboardNavigation *record);
UmiStatus umi_workbench_context_link_keyboard_navigation_set_primary(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    const char *value);
UmiStatus umi_workbench_context_link_keyboard_navigation_set_secondary(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    const char *value);
void umi_workbench_context_link_keyboard_navigation_touch(
    UmiWorkbenchContextLinkKeyboardNavigation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
