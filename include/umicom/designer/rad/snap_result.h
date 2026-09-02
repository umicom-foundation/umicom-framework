/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/snap_result.h
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
#ifndef UMICOM_DESIGNER_RAD_SNAP_RESULT_H
#define UMICOM_DESIGNER_RAD_SNAP_RESULT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad snap result data shared with callers of this public contract.
 */
typedef struct UmiRadSnapResult {
    UmiRadPoint requested;
    UmiRadPoint resolved;
    bool snapped_x;
    bool snapped_y;
} UmiRadSnapResult;
/**
 * Initialise rad snap result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item);
/**
 * Check that rad snap result satisfies its contract before another service relies on it.
 */
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item);
#ifdef __cplusplus
}
#endif
#endif
