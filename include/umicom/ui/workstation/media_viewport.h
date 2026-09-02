/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/media_viewport.h
 *
 * PURPOSE:
 *   Define reusable media viewport state for video editing, AI generation previews, compositing and review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_MEDIA_VIEWPORT_H
#define UMICOM_UI_WORKSTATION_MEDIA_VIEWPORT_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ws viewport fit values accepted by this public contract.
 */
typedef enum UmiWsViewportFit {
    UMI_WS_VIEWPORT_FIT = 1,
    UMI_WS_VIEWPORT_FILL = 2,
    UMI_WS_VIEWPORT_ONE_TO_ONE = 3
} UmiWsViewportFit;

/**
 * Represent the ws media viewport data shared with callers of this public contract.
 */
typedef struct UmiWsMediaViewport {
    char viewport_id[UMI_UI_ID_CAPACITY];
    int32_t source_width;
    int32_t source_height;
    double zoom;
    UmiWsViewportFit fit;
    bool show_safe_area;
    bool show_alpha;
} UmiWsMediaViewport;

/**
 * Initialise ws media viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_media_viewport_init(UmiWsMediaViewport *viewport,
                                     const char *viewport_id,
                                     int32_t source_width,
                                     int32_t source_height);
/**
 * Provide the ws media viewport set zoom operation used by this module and its client
 * applications.
 */
double umi_ws_media_viewport_set_zoom(UmiWsMediaViewport *viewport, double zoom);
/**
 * Provide the ws media viewport set fit operation used by this module and its client
 * applications.
 */
void umi_ws_media_viewport_set_fit(UmiWsMediaViewport *viewport, UmiWsViewportFit fit);

#ifdef __cplusplus
}
#endif

#endif
