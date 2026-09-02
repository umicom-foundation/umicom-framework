/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/parent_constraint.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PARENT_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PARENT_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer parent constraint data shared with callers of this public contract.
 */
typedef struct UmiRadParentConstraint {
    char parent_type[UMI_RAD_ID_CAPACITY];
    char child_family[UMI_RAD_ID_CAPACITY];
    bool accepted;
} UmiRadParentConstraint;
/**
 * Initialise visual designer parent constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item);
/**
 * Check that visual designer parent constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item);
#ifdef __cplusplus
}
#endif
#endif
