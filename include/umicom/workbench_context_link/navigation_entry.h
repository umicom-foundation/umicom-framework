/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/navigation_entry.h
 *
 * PURPOSE:
 *   Define the reusable context navigation stack entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_NAVIGATION_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_NAVIGATION_ENTRY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkNavigationEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkNavigationEntry;

void umi_workbench_context_link_navigation_entry_init(UmiWorkbenchContextLinkNavigationEntry *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_navigation_entry_validate(
    const UmiWorkbenchContextLinkNavigationEntry *record);
UmiStatus umi_workbench_context_link_navigation_entry_copy(
    UmiWorkbenchContextLinkNavigationEntry *destination,
    const UmiWorkbenchContextLinkNavigationEntry *source);
uint64_t umi_workbench_context_link_navigation_entry_hash(
    const UmiWorkbenchContextLinkNavigationEntry *record);
UmiStatus umi_workbench_context_link_navigation_entry_set_primary(
    UmiWorkbenchContextLinkNavigationEntry *record,
    const char *value);
UmiStatus umi_workbench_context_link_navigation_entry_set_secondary(
    UmiWorkbenchContextLinkNavigationEntry *record,
    const char *value);
void umi_workbench_context_link_navigation_entry_touch(
    UmiWorkbenchContextLinkNavigationEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
