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
typedef struct UmiCashflow { UmiFinancialId cashflow_id; UmiTradeId trade_id; UmiMoney amount; UmiFinancialDirection direction; UmiFinancialDate payment_date; } UmiCashflow;
/* Initialize cashflow. */ UmiStatus umi_cashflow_init(UmiCashflow *c,const char *id,const char *trade,UmiMoney amount,UmiFinancialDirection dir,UmiFinancialDate date);
/* Validate cashflow. */ bool umi_cashflow_is_valid(const UmiCashflow *c);
#ifdef __cplusplus
}
#endif

#endif
