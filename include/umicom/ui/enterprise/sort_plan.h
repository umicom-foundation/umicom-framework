/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sort_plan.h
 *
 * PURPOSE:
 *   Maintain deterministic priority-ordered multi-column sort descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SORT_PLAN_H
#define UMICOM_UI_ENTERPRISE_SORT_PLAN_H

#include "umicom/ui/enterprise/sort_descriptor.h"
/**
 * Represent the ui ent sort plan data shared with callers of this public contract.
 */
typedef struct UmiUiEntSortPlan { UmiUiEntSortDescriptor items[UMI_UI_ENT_MAX_RULES]; size_t count; } UmiUiEntSortPlan;
/**
 * Initialise ui ent sort plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_sort_plan_init(UmiUiEntSortPlan *p);
/**
 * Add ui ent sort plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_ent_sort_plan_add(UmiUiEntSortPlan *p,const UmiUiEntSortDescriptor *item);
/**
 * Find ui ent sort plan while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiEntSortDescriptor *umi_ui_ent_sort_plan_at(const UmiUiEntSortPlan *p,size_t index);

#endif
