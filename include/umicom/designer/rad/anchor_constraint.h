/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/anchor_constraint.h
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_ANCHOR_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_ANCHOR_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadAnchorConstraint {
    bool left;
    bool top;
    bool right;
    bool bottom;
    int32_t margin;
} UmiRadAnchorConstraint;
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item);
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
