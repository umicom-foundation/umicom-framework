/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_calendar.h
 *
 * PURPOSE:
 *   Maintain non-overlapping accounting periods and resolve a period for a business date.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_CALENDAR_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_CALENDAR_H
#include "umicom/finance/accounting/accounting_period.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingAccountingCalendar { UmiAccountingAccountingPeriod items[UMI_ACCOUNTING_MAX_ITEMS]; size_t count; } UmiAccountingAccountingCalendar;
void umi_accounting_accounting_calendar_init(UmiAccountingAccountingCalendar *value);
UmiStatus umi_accounting_accounting_calendar_add(UmiAccountingAccountingCalendar *value,const UmiAccountingAccountingPeriod *period);
const UmiAccountingAccountingPeriod *umi_accounting_accounting_calendar_find_date(const UmiAccountingAccountingCalendar *value,UmiFinancialDate date);
#ifdef __cplusplus
}
#endif
#endif
