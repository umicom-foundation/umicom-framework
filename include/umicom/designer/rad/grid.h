/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/grid.h
 *
 * PURPOSE:
 *   Describe configurable design-time grid spacing and visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_GRID_H
#define UMICOM_DESIGNER_RAD_GRID_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad design grid data shared with callers of this public contract.
 */
typedef struct UmiRadDesignGrid {
    int32_t spacing_x;
    int32_t spacing_y;
    bool visible;
    bool snap_enabled;
} UmiRadDesignGrid;
/**
 * Initialise rad grid from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_grid_init(UmiRadDesignGrid *item);
/**
 * Check that rad grid satisfies its contract before another service relies on it.
 */
int umi_rad_grid_is_valid(const UmiRadDesignGrid *item);
#ifdef __cplusplus
}
#endif
#endif
