/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sort_plan.c
 *
 * PURPOSE:
 *   Implement stable priority insertion for sort plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_plan.h"
#include <string.h>
/*
 * Initialise ui ent sort plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_sort_plan_init(UmiUiEntSortPlan *p){/* Apply this branch only when its contract condition is satisfied. */ if(p)memset(p,0,sizeof *p);}
/* Add ui ent sort plan only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_ent_sort_plan_add(UmiUiEntSortPlan *p,const UmiUiEntSortDescriptor *x){size_t pos,i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!umi_ui_ent_sort_descriptor_validate(x))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;pos=p->count;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<p->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(x->priority<p->items[i].priority){pos=i;break;}/* Visit each bounded item once so every record receives the same rule. */ for(i=p->count;i>pos;i--)p->items[i]=p->items[i-1U];p->items[pos]=*x;p->count++;return UMI_STATUS_OK;}
/*
 * Find ui ent sort plan while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiEntSortDescriptor *umi_ui_ent_sort_plan_at(const UmiUiEntSortPlan *p,size_t i){return p&&i<p->count?&p->items[i]:NULL;}
