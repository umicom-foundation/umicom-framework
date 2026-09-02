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
/**
 * Represent the accounting financial control data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingFinancialControl {
    UmiFinancialId id;
    size_t control_breaches;
    size_t unreconciled_items;
    size_t pending_approvals;
} UmiAccountingFinancialControl;
/**
 * Initialise accounting financial control from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_financial_control_init(UmiAccountingFinancialControl *value,
    const char *id,
    size_t control_breaches,
    size_t unreconciled_items,
    size_t pending_approvals);
/**
 * Check that accounting financial control satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_financial_control_valid(const UmiAccountingFinancialControl *value);
/**
 * Provide the accounting financial control healthy operation used by this module and its
 * client applications.
 */
bool umi_accounting_financial_control_healthy(const UmiAccountingFinancialControl *value);
#ifdef __cplusplus
}
#endif
#endif
