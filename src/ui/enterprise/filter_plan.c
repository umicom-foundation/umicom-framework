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
/*
 * Initialise ui ent filter plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_filter_plan_init(UmiUiEntFilterPlan *p){/* Apply this branch only when its contract condition is satisfied. */ if(p)memset(p,0,sizeof *p);}
/* Add ui ent filter plan only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_ent_filter_plan_add(UmiUiEntFilterPlan *p,const UmiUiEntFilterPredicate *x){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!umi_ui_ent_filter_predicate_validate(x))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;p->items[p->count++]=*x;p->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent filter plan matches one operation used by this module and its client
 * applications.
 */
int umi_ui_ent_filter_plan_matches_one(const UmiUiEntFilterPlan *p,const char *column,const UmiUiEntCellValue *v){size_t i;int saw=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!column||!v)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<p->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(p->items[i].column_id,column)==0){saw=1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_ui_ent_filter_predicate_matches(&p->items[i],v))return 0;}return saw?1:1;}
