/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/duplicate_plan.h
 *
 * PURPOSE:
 *   Describe deterministic component duplication before it is committed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DUPLICATE_PLAN_H
#define UMICOM_DESIGNER_RAD_DUPLICATE_PLAN_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDuplicatePlan {
    char source_id[UMI_RAD_ID_CAPACITY];
    char new_id[UMI_RAD_ID_CAPACITY];
    char new_parent_id[UMI_RAD_ID_CAPACITY];
    UmiRadPoint offset;
} UmiRadDuplicatePlan;
UmiStatus umi_rad_duplicate_plan_init(UmiRadDuplicatePlan *item);
int umi_rad_duplicate_plan_is_valid(const UmiRadDuplicatePlan *item);
#ifdef __cplusplus
}
#endif
#endif
