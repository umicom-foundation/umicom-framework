/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/layout_constraint.h
 *
 * PURPOSE:
 *   Describe minimum/maximum geometry constraints for designer components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_LAYOUT_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_LAYOUT_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadLayoutConstraint {
    int32_t min_width;
    int32_t max_width;
    int32_t min_height;
    int32_t max_height;
} UmiRadLayoutConstraint;
UmiStatus umi_rad_layout_constraint_init(UmiRadLayoutConstraint *item);
int umi_rad_layout_constraint_is_valid(const UmiRadLayoutConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
