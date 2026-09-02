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
/**
 * Represent the rad selection bounds data shared with callers of this public contract.
 */
typedef struct UmiRadSelectionBounds {
    UmiRadRect bounds;
    size_t item_count;
} UmiRadSelectionBounds;
/**
 * Provide the rad selection bounds compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_selection_bounds_compute(const UmiRadRect *rects,size_t count,UmiRadSelectionBounds *out_bounds);
#ifdef __cplusplus
}
#endif
#endif
