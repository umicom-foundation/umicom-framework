/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/dock_preview.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral dock preview geometry for visual drop-target overlays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/dock_preview.h"

/*
 * Provide the ws dock preview compute operation used by this module and its client
 * applications.
 */
UmiWsDockPreview umi_ws_dock_preview_compute(UmiUiRect host, UmiWsDockRegion region, double split_fraction) {
    UmiWsDockPreview preview = {0};
    double fraction = split_fraction;
    /* Apply this branch only when its contract condition is satisfied. */
    if (fraction < 0.15) fraction = 0.15;
    /* Apply this branch only when its contract condition is satisfied. */
    if (fraction > 0.5) fraction = 0.5;
    preview.region = region;
    preview.bounds = host;
    preview.opacity = 0.28;
    preview.visible = host.width > 0 && host.height > 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (region == UMI_WS_DOCK_LEFT || region == UMI_WS_DOCK_RIGHT) {
        preview.bounds.width = (int32_t)((double)host.width * fraction);
        /* Apply this branch only when its contract condition is satisfied. */
        if (region == UMI_WS_DOCK_RIGHT) preview.bounds.x = host.x + host.width - preview.bounds.width;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (region == UMI_WS_DOCK_TOP || region == UMI_WS_DOCK_BOTTOM) {
        preview.bounds.height = (int32_t)((double)host.height * fraction);
        /* Apply this branch only when its contract condition is satisfied. */
        if (region == UMI_WS_DOCK_BOTTOM) preview.bounds.y = host.y + host.height - preview.bounds.height;
    }
    return preview;
}
