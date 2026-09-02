/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/grid_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_GRID_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_GRID_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad grid constraint data shared with callers of this public contract.
 */
typedef struct UmiRadGridConstraint {
    int32_t row;
    int32_t column;
    int32_t row_span;
    int32_t column_span;
} UmiRadGridConstraint;
/**
 * Initialise rad grid constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item);
/**
 * Check that rad grid constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
