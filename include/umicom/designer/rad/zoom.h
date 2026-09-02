/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/zoom.h
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
#ifndef UMICOM_DESIGNER_RAD_ZOOM_H
#define UMICOM_DESIGNER_RAD_ZOOM_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad zoom policy data shared with callers of this public contract.
 */
typedef struct UmiRadZoomPolicy {
    double minimum;
    double maximum;
    double current;
} UmiRadZoomPolicy;
/**
 * Initialise rad zoom from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item);
/**
 * Check that rad zoom satisfies its contract before another service relies on it.
 */
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
