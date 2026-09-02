/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/filter_plan.h
 *
 * PURPOSE:
 *   Maintain a conjunctive set of enterprise filter predicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FILTER_PLAN_H
#define UMICOM_UI_ENTERPRISE_FILTER_PLAN_H

#include "umicom/ui/enterprise/filter_predicate.h"
/**
 * Represent the ui ent filter plan data shared with callers of this public contract.
 */
typedef struct UmiUiEntFilterPlan { UmiUiEntFilterPredicate items[UMI_UI_ENT_MAX_RULES]; size_t count; uint64_t revision; } UmiUiEntFilterPlan;
/**
 * Initialise ui ent filter plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_filter_plan_init(UmiUiEntFilterPlan *p);
/**
 * Add ui ent filter plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_ent_filter_plan_add(UmiUiEntFilterPlan *p,const UmiUiEntFilterPredicate *item);
/**
 * Provide the ui ent filter plan matches one operation used by this module and its client
 * applications.
 */
int umi_ui_ent_filter_plan_matches_one(const UmiUiEntFilterPlan *p,const char *column_id,const UmiUiEntCellValue *value);

#endif
