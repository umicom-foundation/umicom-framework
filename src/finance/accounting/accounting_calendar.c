/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/accounting_calendar.c
 *
 * PURPOSE:
 *   Implement non-overlapping accounting calendar insertion and date resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_calendar.h"
#include <string.h>
void umi_accounting_accounting_calendar_init(UmiAccountingAccountingCalendar *value){ if(value!=NULL)memset(value,0,sizeof *value); }
UmiStatus umi_accounting_accounting_calendar_add(UmiAccountingAccountingCalendar *value,const UmiAccountingAccountingPeriod *period){ if(value==NULL||period==NULL||!umi_accounting_accounting_period_valid(period))return UMI_STATUS_INVALID_ARGUMENT; if(value->count>=UMI_ACCOUNTING_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; if(umi_financial_date_compare(period->start_date,p->end_date)<=0 && umi_financial_date_compare(period->end_date,p->start_date)>=0)return UMI_STATUS_ALREADY_EXISTS; } value->items[value->count++]=*period; return UMI_STATUS_OK; }
const UmiAccountingAccountingPeriod *umi_accounting_accounting_calendar_find_date(const UmiAccountingAccountingCalendar *value,UmiFinancialDate date){ if(value==NULL||!umi_financial_date_is_valid(date))return NULL; for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; if(umi_financial_date_compare(date,p->start_date)>=0&&umi_financial_date_compare(date,p->end_date)<=0)return p; } return NULL; }
