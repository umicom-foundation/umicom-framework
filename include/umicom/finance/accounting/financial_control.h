/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/financial_control.h
 *
 * PURPOSE:
 *   Aggregate accounting control breaches, unreconciled items and pending approvals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_FINANCIAL_CONTROL_H
#define UMICOM_FINANCE_ACCOUNTING_FINANCIAL_CONTROL_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingFinancialControl {
    UmiFinancialId id;
    size_t control_breaches;
    size_t unreconciled_items;
    size_t pending_approvals;
} UmiAccountingFinancialControl;
UmiStatus umi_accounting_financial_control_init(UmiAccountingFinancialControl *value,
    const char *id,
    size_t control_breaches,
    size_t unreconciled_items,
    size_t pending_approvals);
bool umi_accounting_financial_control_valid(const UmiAccountingFinancialControl *value);
bool umi_accounting_financial_control_healthy(const UmiAccountingFinancialControl *value);
#ifdef __cplusplus
}
#endif
#endif
