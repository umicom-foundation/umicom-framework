/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/cashflow_schedule.c
 *
 * PURPOSE:
 *   Implement bounded cashflow schedules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/cashflow_schedule.h"

#include <string.h>
/* Reset collection. */ void umi_cashflow_schedule_init(UmiCashflowSchedule *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_cashflow_schedule_add(UmiCashflowSchedule *c,const UmiCashflow *item){if(c==NULL||item==NULL||!umi_cashflow_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_cashflow_schedule_count(const UmiCashflowSchedule *c){return c!=NULL?c->count:0U;}
