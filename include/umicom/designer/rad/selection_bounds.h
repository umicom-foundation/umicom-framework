/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/selection_bounds.h
 *
 * PURPOSE:
 *   Calculate the bounding rectangle of a multi-component selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_SELECTION_BOUNDS_H
#define UMICOM_DESIGNER_RAD_SELECTION_BOUNDS_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadSelectionBounds {
    UmiRadRect bounds;
    size_t item_count;
} UmiRadSelectionBounds;
UmiStatus umi_rad_selection_bounds_compute(const UmiRadRect *rects,size_t count,UmiRadSelectionBounds *out_bounds);
#ifdef __cplusplus
}
#endif
#endif
