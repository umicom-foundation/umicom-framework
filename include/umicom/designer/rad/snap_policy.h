/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/snap_policy.h
 *
 * PURPOSE:
 *   Configure grid, guide and component snapping tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_SNAP_POLICY_H
#define UMICOM_DESIGNER_RAD_SNAP_POLICY_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadSnapPolicy {
    bool grid_enabled;
    bool guides_enabled;
    bool components_enabled;
    int32_t tolerance;
} UmiRadSnapPolicy;
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item);
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
