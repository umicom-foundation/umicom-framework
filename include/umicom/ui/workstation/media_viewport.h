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

typedef enum UmiWsViewportFit {
    UMI_WS_VIEWPORT_FIT = 1,
    UMI_WS_VIEWPORT_FILL = 2,
    UMI_WS_VIEWPORT_ONE_TO_ONE = 3
} UmiWsViewportFit;

typedef struct UmiWsMediaViewport {
    char viewport_id[UMI_UI_ID_CAPACITY];
    int32_t source_width;
    int32_t source_height;
    double zoom;
    UmiWsViewportFit fit;
    bool show_safe_area;
    bool show_alpha;
} UmiWsMediaViewport;

UmiStatus umi_ws_media_viewport_init(UmiWsMediaViewport *viewport,
                                     const char *viewport_id,
                                     int32_t source_width,
                                     int32_t source_height);
double umi_ws_media_viewport_set_zoom(UmiWsMediaViewport *viewport, double zoom);
void umi_ws_media_viewport_set_fit(UmiWsMediaViewport *viewport, UmiWsViewportFit fit);

#ifdef __cplusplus
}
#endif

#endif
