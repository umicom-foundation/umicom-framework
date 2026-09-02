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
/*
 * Initialise accounting accounting calendar from caller-provided values so later
 * operations receive a known state.
 */
void umi_accounting_accounting_calendar_init(UmiAccountingAccountingCalendar *value){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL)memset(value,0,sizeof *value); }
/*
 * Add accounting accounting calendar only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_accounting_accounting_calendar_add(UmiAccountingAccountingCalendar *value,const UmiAccountingAccountingPeriod *period){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||period==NULL||!umi_accounting_accounting_period_valid(period))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->count>=UMI_ACCOUNTING_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_date_compare(period->start_date,p->end_date)<=0 && umi_financial_date_compare(period->end_date,p->start_date)>=0)return UMI_STATUS_ALREADY_EXISTS; } value->items[value->count++]=*period; return UMI_STATUS_OK; }
/*
 * Provide the accounting accounting calendar find date operation used by this module and
 * its client applications.
 */
const UmiAccountingAccountingPeriod *umi_accounting_accounting_calendar_find_date(const UmiAccountingAccountingCalendar *value,UmiFinancialDate date){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||!umi_financial_date_is_valid(date))return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_date_compare(date,p->start_date)>=0&&umi_financial_date_compare(date,p->end_date)<=0)return p; } return NULL; }
