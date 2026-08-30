/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/parent_constraint.h
 *
 * PURPOSE:
 *   Describe which semantic component families a parent slot accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PARENT_CONSTRAINT_H
#define UMICOM_DESIGNER_RAD_PARENT_CONSTRAINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadParentConstraint {
    char parent_type[UMI_RAD_ID_CAPACITY];
    char child_family[UMI_RAD_ID_CAPACITY];
    bool accepted;
} UmiRadParentConstraint;
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item);
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
