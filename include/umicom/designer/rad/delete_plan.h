/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/delete_plan.h
 *
 * PURPOSE:
 *   Describe a bounded set of component identifiers scheduled for deletion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DELETE_PLAN_H
#define UMICOM_DESIGNER_RAD_DELETE_PLAN_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad delete plan data shared with callers of this public contract.
 */
typedef struct UmiRadDeletePlan { char ids[UMI_RAD_MAX_ITEMS][UMI_RAD_ID_CAPACITY]; size_t count; } UmiRadDeletePlan;
/**
 * Initialise rad delete plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_delete_plan_init(UmiRadDeletePlan *plan);
/**
 * Add rad delete plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_rad_delete_plan_add(UmiRadDeletePlan *plan,const char *component_id);
/**
 * Provide the rad delete plan contains operation used by this module and its client
 * applications.
 */
int umi_rad_delete_plan_contains(const UmiRadDeletePlan *plan,const char *component_id);
#ifdef __cplusplus
}
#endif
#endif
