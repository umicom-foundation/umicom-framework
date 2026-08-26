/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/grid_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_GRID_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_GRID_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadGridConstraint {
    int32_t row;
    int32_t column;
    int32_t row_span;
    int32_t column_span;
} UmiRadGridConstraint;
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item);
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
