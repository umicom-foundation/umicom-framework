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
typedef struct UmiRadZoomPolicy {
    double minimum;
    double maximum;
    double current;
} UmiRadZoomPolicy;
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item);
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
