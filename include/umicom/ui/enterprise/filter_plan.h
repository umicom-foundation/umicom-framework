/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/filter_plan.h
 *
 * PURPOSE:
 *   Maintain a conjunctive set of enterprise filter predicates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FILTER_PLAN_H
#define UMICOM_UI_ENTERPRISE_FILTER_PLAN_H

#include "umicom/ui/enterprise/filter_predicate.h"
typedef struct UmiUiEntFilterPlan { UmiUiEntFilterPredicate items[UMI_UI_ENT_MAX_RULES]; size_t count; uint64_t revision; } UmiUiEntFilterPlan;
void umi_ui_ent_filter_plan_init(UmiUiEntFilterPlan *p);
UmiStatus umi_ui_ent_filter_plan_add(UmiUiEntFilterPlan *p,const UmiUiEntFilterPredicate *item);
int umi_ui_ent_filter_plan_matches_one(const UmiUiEntFilterPlan *p,const char *column_id,const UmiUiEntCellValue *value);

#endif
