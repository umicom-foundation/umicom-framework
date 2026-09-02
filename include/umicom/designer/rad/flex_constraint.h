/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/flex_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral flexible-box growth and basis constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_FLEX_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_FLEX_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad flex constraint data shared with callers of this public contract.
 */
typedef struct UmiRadFlexConstraint {
    double grow;
    double shrink;
    int32_t basis;
    int32_t order;
} UmiRadFlexConstraint;
/**
 * Initialise rad flex constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_flex_constraint_init(UmiRadFlexConstraint *item);
/**
 * Check that rad flex constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_flex_constraint_is_valid(const UmiRadFlexConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
