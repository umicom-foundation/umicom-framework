/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/snap_policy.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_POLICY_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_POLICY_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer snap policy data shared with callers of this public contract.
 */
typedef struct UmiRadSnapPolicy {
    bool grid_enabled;
    bool guides_enabled;
    bool components_enabled;
    int32_t tolerance;
} UmiRadSnapPolicy;
/**
 * Initialise visual designer snap policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item);
/**
 * Check that visual designer snap policy satisfies its contract before another service relies on it.
 */
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
