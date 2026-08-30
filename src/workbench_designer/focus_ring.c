/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/focus_ring.c
 *
 * PURPOSE:
 *   Build and navigate a bounded focus ring while preserving the currently
 *   focused semantic node across canvas refreshes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/focus_ring.h"
#include "internal.h"


void umi_workbench_designer_focus_ring_init(UmiWorkbenchDesignerFocusRing *ring)
{
    if (ring == NULL) return;
    (void)memset(ring, 0, sizeof(*ring));
    ring->current_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_find(
    const UmiWorkbenchDesignerFocusRing *ring,
    const char *node_id)
{
    size_t index;
    if (ring == NULL || node_id == NULL) return NULL;
    for (index = 0U; index < ring->count; ++index) {
        if (strcmp(ring->items[index].node_id, node_id) == 0) {
            return &ring->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_designer_focus_ring_build(
    UmiWorkbenchDesignerFocusRing *ring,
    const UmiWorkbenchDesignerCanvas *canvas,
    bool include_containers)
{
    char previous[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    size_t index;
    if (ring == NULL || canvas == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    previous[0] = '\0';
    if (ring->current_index < ring->count) {
        (void)umi_workbench_designer_copy_text(
            previous, sizeof(previous), ring->items[ring->current_index].node_id);
    }
    ring->count = 0U;
    ring->current_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    for (index = 0U; index < canvas->count; ++index) {
        const UmiWorkbenchDesignerCanvasItem *item = &canvas->items[index];
        UmiWorkbenchDesignerFocusItem *focus;
        if (!item->visible || (!include_containers && item->container)) continue;
        if (ring->count >= UMI_WORKBENCH_DESIGNER_MAX_FOCUS_ITEMS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        focus = &ring->items[ring->count];
        (void)memset(focus, 0, sizeof(*focus));
        (void)umi_workbench_designer_copy_text(
            focus->node_id, sizeof(focus->node_id), item->node_id);
        (void)umi_workbench_designer_copy_text(
            focus->label, sizeof(focus->label), item->title);
        focus->canvas_index = index;
        focus->order = (uint32_t)ring->count;
        focus->enabled = true;
        focus->container = item->container;
        if (previous[0] != '\0' && strcmp(previous, focus->node_id) == 0) {
            ring->current_index = ring->count;
        }
        ring->count += 1U;
    }
    if (ring->current_index == UMI_WORKBENCH_DESIGNER_INDEX_NONE && ring->count > 0U) {
        ring->current_index = 0U;
    }
    ring->canvas_revision = canvas->revision;
    ring->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_focus_ring_set(
    UmiWorkbenchDesignerFocusRing *ring,
    const char *node_id)
{
    size_t index;
    if (ring == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < ring->count; ++index) {
        if (ring->items[index].enabled && strcmp(ring->items[index].node_id, node_id) == 0) {
            ring->current_index = index;
            ring->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_current(
    const UmiWorkbenchDesignerFocusRing *ring)
{
    if (ring == NULL || ring->current_index >= ring->count) return NULL;
    return &ring->items[ring->current_index];
}

const UmiWorkbenchDesignerFocusItem *umi_workbench_designer_focus_ring_move(
    UmiWorkbenchDesignerFocusRing *ring,
    int direction,
    bool wrap)
{
    size_t attempts;
    size_t candidate;
    if (ring == NULL || ring->count == 0U || direction == 0) return NULL;
    candidate = ring->current_index < ring->count ? ring->current_index : 0U;
    for (attempts = 0U; attempts < ring->count; ++attempts) {
        if (direction > 0) {
            if (candidate + 1U >= ring->count) {
                if (!wrap) return umi_workbench_designer_focus_ring_current(ring);
                candidate = 0U;
            } else {
                candidate += 1U;
            }
        } else if (candidate == 0U) {
            if (!wrap) return umi_workbench_designer_focus_ring_current(ring);
            candidate = ring->count - 1U;
        } else {
            candidate -= 1U;
        }
        if (ring->items[candidate].enabled) {
            ring->current_index = candidate;
            ring->revision += 1U;
            return &ring->items[candidate];
        }
    }
    return NULL;
}
