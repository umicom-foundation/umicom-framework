/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/brokerage_account.h
 *
 * PURPOSE:
 *   Define brokerage account identity, base currency and account trading state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_H
#define UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingBrokerageAccount { UmiFinancialId account_id; UmiCurrency base_currency; bool enabled; bool margin_enabled; } UmiTradingBrokerageAccount;
/* Initialise a brokerage account. */
UmiStatus umi_trading_brokerage_account_init(UmiTradingBrokerageAccount *account,const char *account_id,const UmiCurrency *base_currency,bool margin_enabled);
/* Validate required account and currency fields. */
bool umi_trading_brokerage_account_valid(const UmiTradingBrokerageAccount *account);
#ifdef __cplusplus
}
#endif
#endif
