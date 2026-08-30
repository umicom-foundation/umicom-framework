/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_plan.c
 *
 * PURPOSE:
 *   Maintain a bounded sequence of planned capital actions by period.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_plan.h"

#include <string.h>
UmiStatus umi_pru_capital_plan_add(UmiPrudentialCapitalPlan *plan,const char *action_id,int32_t period,double amount) { UmiStatus s; UmiPrudentialCapitalPlanItem *item; if(plan==NULL||action_id==NULL||!umi_pru_number_valid(amount)||amount<0.0||period<0)return UMI_STATUS_INVALID_ARGUMENT; if(plan->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; item=&plan->items[plan->count]; memset(item,0,sizeof *item); s=umi_pru_copy_text(item->action_id,sizeof item->action_id,action_id); if(s!=UMI_STATUS_OK)return s; item->period=period; item->amount=amount; ++plan->count; return UMI_STATUS_OK; }
double umi_pru_capital_plan_total(const UmiPrudentialCapitalPlan *plan) { size_t i; double total=0.0; if(plan==NULL)return 0.0; for(i=0U;i<plan->count;++i) total+=plan->items[i].amount; return total; }
