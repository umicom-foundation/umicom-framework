/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/filter_plan.c
 *
 * PURPOSE:
 *   Implement bounded conjunctive filter plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/filter_plan.h"
#include <string.h>
void umi_ui_ent_filter_plan_init(UmiUiEntFilterPlan *p){if(p)memset(p,0,sizeof *p);}
UmiStatus umi_ui_ent_filter_plan_add(UmiUiEntFilterPlan *p,const UmiUiEntFilterPredicate *x){if(!p||!umi_ui_ent_filter_predicate_validate(x))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;p->items[p->count++]=*x;p->revision++;return UMI_STATUS_OK;}
int umi_ui_ent_filter_plan_matches_one(const UmiUiEntFilterPlan *p,const char *column,const UmiUiEntCellValue *v){size_t i;int saw=0;if(!p||!column||!v)return 0;for(i=0;i<p->count;i++)if(strcmp(p->items[i].column_id,column)==0){saw=1;if(!umi_ui_ent_filter_predicate_matches(&p->items[i],v))return 0;}return saw?1:1;}
