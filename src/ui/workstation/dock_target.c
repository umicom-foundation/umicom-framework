/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/dock_target.c
 *
 * PURPOSE:
 *   Implement scored docking targets so GTK4 drag overlays and keyboard docking share one deterministic policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/dock_target.h"

/*
 * Initialise ws dock target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_dock_target_init(UmiWsDockTarget *target,
                                  const char *target_id,
                                  UmiWsDockRegion region,
                                  UmiUiRect bounds) {
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL || !umi_ws_id_valid(target_id) || bounds.width < 0 || bounds.height < 0) return UMI_STATUS_INVALID_ARGUMENT;
    *target = (UmiWsDockTarget){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(target->target_id, sizeof(target->target_id), target_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    target->region = region;
    target->bounds = bounds;
    target->enabled = true;
    target->priority = (region == UMI_WS_DOCK_CENTRE) ? 20 : 10;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws dock target contains operation used by this module and its client
 * applications.
 */
bool umi_ws_dock_target_contains(const UmiWsDockTarget *target, UmiUiPoint pointer) {
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL || !target->enabled) return false;
    return pointer.x >= target->bounds.x &&
           pointer.y >= target->bounds.y &&
           pointer.x < target->bounds.x + target->bounds.width &&
           pointer.y < target->bounds.y + target->bounds.height;
}

/*
 * Provide the ws dock target score operation used by this module and its client
 * applications.
 */
int32_t umi_ws_dock_target_score(const UmiWsDockTarget *target, UmiUiPoint pointer) {
    int32_t centre_x;
    int32_t centre_y;
    int32_t dx;
    int32_t dy;
    int32_t distance;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_dock_target_contains(target, pointer)) return -1;
    centre_x = target->bounds.x + target->bounds.width / 2;
    centre_y = target->bounds.y + target->bounds.height / 2;
    dx = pointer.x - centre_x;
    dy = pointer.y - centre_y;
    /* Apply this branch only when its contract condition is satisfied. */
    if (dx < 0) dx = -dx;
    /* Apply this branch only when its contract condition is satisfied. */
    if (dy < 0) dy = -dy;
    distance = dx + dy;
    return target->priority * 1000 - distance;
}
