/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sort_plan.h
 *
 * PURPOSE:
 *   Maintain deterministic priority-ordered multi-column sort descriptors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SORT_PLAN_H
#define UMICOM_UI_ENTERPRISE_SORT_PLAN_H

#include "umicom/ui/enterprise/sort_descriptor.h"
typedef struct UmiUiEntSortPlan { UmiUiEntSortDescriptor items[UMI_UI_ENT_MAX_RULES]; size_t count; } UmiUiEntSortPlan;
void umi_ui_ent_sort_plan_init(UmiUiEntSortPlan *p);
UmiStatus umi_ui_ent_sort_plan_add(UmiUiEntSortPlan *p,const UmiUiEntSortDescriptor *item);
const UmiUiEntSortDescriptor *umi_ui_ent_sort_plan_at(const UmiUiEntSortPlan *p,size_t index);

#endif
