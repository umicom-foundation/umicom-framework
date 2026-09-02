/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/brokerage_account.c
 *
 * PURPOSE:
 *   Define brokerage account identity, base currency and account trading state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/brokerage_account.h"

#include <string.h>
/*
 * Initialise trading brokerage account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_brokerage_account_init(UmiTradingBrokerageAccount *account,const char *account_id,const UmiCurrency *base_currency,bool margin_enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(account==NULL||base_currency==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(account,0,sizeof *account);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_assign(&account->account_id,account_id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;account->base_currency=*base_currency;account->enabled=true;account->margin_enabled=margin_enabled;return umi_trading_brokerage_account_valid(account)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Check that trading brokerage account satisfies its contract before another service
 * relies on it.
 */
bool umi_trading_brokerage_account_valid(const UmiTradingBrokerageAccount *account){return account!=NULL&&account->account_id.value[0]!='\0'&&account->base_currency.code[0]!='\0';}
