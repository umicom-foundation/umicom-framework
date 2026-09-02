/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/snap_result.h
 *
 * PURPOSE:
 *   Record the deterministic outcome of a snap calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_RESULT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_RESULT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer snap result data shared with callers of this public contract.
 */
typedef struct UmiRadSnapResult {
    UmiRadPoint requested;
    UmiRadPoint resolved;
    bool snapped_x;
    bool snapped_y;
} UmiRadSnapResult;
/**
 * Initialise visual designer snap result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item);
/**
 * Check that visual designer snap result satisfies its contract before another service relies on it.
 */
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item);
#ifdef __cplusplus
}
#endif
#endif
