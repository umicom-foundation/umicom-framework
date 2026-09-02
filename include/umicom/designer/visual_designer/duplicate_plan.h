/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/duplicate_plan.h
 *
 * PURPOSE:
 *   Describe deterministic component duplication before it is committed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DUPLICATE_PLAN_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DUPLICATE_PLAN_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer duplicate plan data shared with callers of this public contract.
 */
typedef struct UmiRadDuplicatePlan {
    char source_id[UMI_RAD_ID_CAPACITY];
    char new_id[UMI_RAD_ID_CAPACITY];
    char new_parent_id[UMI_RAD_ID_CAPACITY];
    UmiRadPoint offset;
} UmiRadDuplicatePlan;
/**
 * Initialise visual designer duplicate plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_duplicate_plan_init(UmiRadDuplicatePlan *item);
/**
 * Check that visual designer duplicate plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_duplicate_plan_is_valid(const UmiRadDuplicatePlan *item);
#ifdef __cplusplus
}
#endif
#endif
