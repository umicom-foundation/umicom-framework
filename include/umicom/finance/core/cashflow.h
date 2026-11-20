/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/cashflow.h
 *
 * PURPOSE:
 *   Represent dated directional cashflows using the existing precise UmiMoney type.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CASHFLOW_H
#define UMICOM_FINANCE_CORE_CASHFLOW_H

#include "umicom/finance/core/trade_id.h"
#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the cashflow data shared with callers of this public contract.
 */
typedef struct UmiCashflow { UmiFinancialId cashflow_id; UmiTradeId trade_id; UmiMoney amount; UmiFinancialDirection direction; UmiFinancialDate payment_date; } UmiCashflow;
/* Stage a complete dated cashflow with a bounded currency and valid trade
 * reference. Direction must be PAY or RECEIVE. Money uses scale 0..9.
 * Invalid input leaves the previous cashflow unchanged. */
UmiStatus umi_cashflow_init(UmiCashflow *cashflow, const char *id, const char *trade,
    UmiMoney amount, UmiFinancialDirection direction, UmiFinancialDate date);
/* Validate a complete owned value before it enters a schedule. */
bool umi_cashflow_is_valid(const UmiCashflow *cashflow);
#ifdef __cplusplus
}
#endif

#endif
