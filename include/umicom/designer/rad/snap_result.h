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
typedef struct UmiRadSnapResult {
    UmiRadPoint requested;
    UmiRadPoint resolved;
    bool snapped_x;
    bool snapped_y;
} UmiRadSnapResult;
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item);
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item);
#ifdef __cplusplus
}
#endif
#endif
