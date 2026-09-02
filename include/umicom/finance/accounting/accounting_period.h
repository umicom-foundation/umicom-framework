/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_period.h
 *
 * PURPOSE:
 *   Represent accounting period boundaries and posting status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_PERIOD_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_PERIOD_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting accounting period data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingAccountingPeriod {
    UmiFinancialId id;
    UmiFinancialDate start_date;
    UmiFinancialDate end_date;
    UmiAccountingPeriodStatus status;
} UmiAccountingAccountingPeriod;
/**
 * Initialise accounting accounting period from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_accounting_period_init(UmiAccountingAccountingPeriod *value,
    const char *id,
    UmiFinancialDate start_date,
    UmiFinancialDate end_date,
    UmiAccountingPeriodStatus status);
/**
 * Check that accounting accounting period satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_accounting_period_valid(const UmiAccountingAccountingPeriod *value);
/**
 * Provide the accounting accounting period open operation used by this module and its
 * client applications.
 */
bool umi_accounting_accounting_period_open(const UmiAccountingAccountingPeriod *value);
#ifdef __cplusplus
}
#endif
#endif
