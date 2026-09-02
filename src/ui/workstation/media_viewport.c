/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/media_viewport.c
 *
 * PURPOSE:
 *   Implement reusable media viewport state for video editing, AI generation previews, compositing and review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/media_viewport.h"

/*
 * Initialise ws media viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_media_viewport_init(UmiWsMediaViewport *viewport,
                                     const char *viewport_id,
                                     int32_t source_width,
                                     int32_t source_height) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (viewport == NULL || !umi_ws_id_valid(viewport_id) || source_width <= 0 || source_height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    *viewport = (UmiWsMediaViewport){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(viewport->viewport_id, sizeof(viewport->viewport_id), viewport_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    viewport->source_width = source_width;
    viewport->source_height = source_height;
    viewport->zoom = 1.0;
    viewport->fit = UMI_WS_VIEWPORT_FIT;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws media viewport set zoom operation used by this module and its client
 * applications.
 */
double umi_ws_media_viewport_set_zoom(UmiWsMediaViewport *viewport, double zoom) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (viewport == NULL) return 0.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom < 0.05) zoom = 0.05;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom > 32.0) zoom = 32.0;
    viewport->zoom = zoom;
    return zoom;
}

/*
 * Provide the ws media viewport set fit operation used by this module and its client
 * applications.
 */
void umi_ws_media_viewport_set_fit(UmiWsMediaViewport *viewport, UmiWsViewportFit fit) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (viewport != NULL && fit >= UMI_WS_VIEWPORT_FIT && fit <= UMI_WS_VIEWPORT_ONE_TO_ONE) viewport->fit = fit;
}
