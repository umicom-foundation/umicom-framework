/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sort_plan.c
 *
 * PURPOSE:
 *   Implement stable priority insertion for sort plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_plan.h"
#include <string.h>
void umi_ui_ent_sort_plan_init(UmiUiEntSortPlan *p){if(p)memset(p,0,sizeof *p);}
UmiStatus umi_ui_ent_sort_plan_add(UmiUiEntSortPlan *p,const UmiUiEntSortDescriptor *x){size_t pos,i;if(!p||!umi_ui_ent_sort_descriptor_validate(x))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;pos=p->count;for(i=0;i<p->count;i++)if(x->priority<p->items[i].priority){pos=i;break;}for(i=p->count;i>pos;i--)p->items[i]=p->items[i-1U];p->items[pos]=*x;p->count++;return UMI_STATUS_OK;}
const UmiUiEntSortDescriptor *umi_ui_ent_sort_plan_at(const UmiUiEntSortPlan *p,size_t i){return p&&i<p->count?&p->items[i]:NULL;}
