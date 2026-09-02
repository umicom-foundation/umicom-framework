/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/anchor_constraint.h
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ANCHOR_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ANCHOR_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer anchor constraint data shared with callers of this public contract.
 */
typedef struct UmiRadAnchorConstraint {
    bool left;
    bool top;
    bool right;
    bool bottom;
    int32_t margin;
} UmiRadAnchorConstraint;
/**
 * Initialise visual designer anchor constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item);
/**
 * Check that visual designer anchor constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
