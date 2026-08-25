/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_period.h
 *
 * PURPOSE:
 *   Represent accounting period boundaries and posting status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_PERIOD_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_PERIOD_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingAccountingPeriod {
    UmiFinancialId id;
    UmiFinancialDate start_date;
    UmiFinancialDate end_date;
    UmiAccountingPeriodStatus status;
} UmiAccountingAccountingPeriod;
UmiStatus umi_accounting_accounting_period_init(UmiAccountingAccountingPeriod *value,
    const char *id,
    UmiFinancialDate start_date,
    UmiFinancialDate end_date,
    UmiAccountingPeriodStatus status);
bool umi_accounting_accounting_period_valid(const UmiAccountingAccountingPeriod *value);
bool umi_accounting_accounting_period_open(const UmiAccountingAccountingPeriod *value);
#ifdef __cplusplus
}
#endif
#endif
