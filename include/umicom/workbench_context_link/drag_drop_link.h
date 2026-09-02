/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/drag_drop_link.h
 *
 * PURPOSE:
 *   Define the reusable drag-and-drop context-link operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DRAG_DROP_LINK_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DRAG_DROP_LINK_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link drag drop link data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkDragDropLink {
    uint32_t structure_size;
    char operation_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char target_panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkDragDropLink;

/**
 * Initialise workbench context link drag drop link from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_drag_drop_link_init(UmiWorkbenchContextLinkDragDropLink *record,
                                           const char *identity);
/**
 * Check that workbench context link drag drop link satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_validate(
    const UmiWorkbenchContextLinkDragDropLink *record);
/**
 * Copy workbench context link drag drop link into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_copy(
    UmiWorkbenchContextLinkDragDropLink *destination,
    const UmiWorkbenchContextLinkDragDropLink *source);
/**
 * Provide the workbench context link drag drop link hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_drag_drop_link_hash(
    const UmiWorkbenchContextLinkDragDropLink *record);
/**
 * Provide the workbench context link drag drop link set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_set_primary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value);
/**
 * Provide the workbench context link drag drop link set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_set_secondary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value);
/**
 * Provide the workbench context link drag drop link touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_drag_drop_link_touch(
    UmiWorkbenchContextLinkDragDropLink *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
