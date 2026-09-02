/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/layout_constraint.h
 *
 * PURPOSE:
 *   Describe minimum/maximum geometry constraints for designer components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_LAYOUT_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_LAYOUT_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer layout constraint data shared with callers of this public contract.
 */
typedef struct UmiRadLayoutConstraint {
    int32_t min_width;
    int32_t max_width;
    int32_t min_height;
    int32_t max_height;
} UmiRadLayoutConstraint;
/**
 * Initialise visual designer layout constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_layout_constraint_init(UmiRadLayoutConstraint *item);
/**
 * Check that visual designer layout constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_layout_constraint_is_valid(const UmiRadLayoutConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
