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
/**
 * Represent the rad parent constraint data shared with callers of this public contract.
 */
typedef struct UmiRadParentConstraint {
    char parent_type[UMI_RAD_ID_CAPACITY];
    char child_family[UMI_RAD_ID_CAPACITY];
    bool accepted;
} UmiRadParentConstraint;
/**
 * Initialise rad parent constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item);
/**
 * Check that rad parent constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
