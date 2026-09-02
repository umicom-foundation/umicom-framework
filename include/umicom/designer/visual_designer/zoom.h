/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/zoom.h
 *
 * PURPOSE:
 *   Provide bounded zoom policy for visual authoring surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ZOOM_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ZOOM_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer zoom policy data shared with callers of this public contract.
 */
typedef struct UmiRadZoomPolicy {
    double minimum;
    double maximum;
    double current;
} UmiRadZoomPolicy;
/**
 * Initialise visual designer zoom from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item);
/**
 * Check that visual designer zoom satisfies its contract before another service relies on it.
 */
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
