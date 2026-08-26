/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/delete_plan.h
 *
 * PURPOSE:
 *   Describe a bounded set of component identifiers scheduled for deletion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DELETE_PLAN_H
#define UMICOM_DESIGNER_RAD_DELETE_PLAN_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDeletePlan { char ids[UMI_RAD_MAX_ITEMS][UMI_RAD_ID_CAPACITY]; size_t count; } UmiRadDeletePlan;
UmiStatus umi_rad_delete_plan_init(UmiRadDeletePlan *plan);
UmiStatus umi_rad_delete_plan_add(UmiRadDeletePlan *plan,const char *component_id);
int umi_rad_delete_plan_contains(const UmiRadDeletePlan *plan,const char *component_id);
#ifdef __cplusplus
}
#endif
#endif
