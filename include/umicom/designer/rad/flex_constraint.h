/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/flex_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral flexible-box growth and basis constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_FLEX_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_FLEX_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadFlexConstraint {
    double grow;
    double shrink;
    int32_t basis;
    int32_t order;
} UmiRadFlexConstraint;
UmiStatus umi_rad_flex_constraint_init(UmiRadFlexConstraint *item);
int umi_rad_flex_constraint_is_valid(const UmiRadFlexConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
