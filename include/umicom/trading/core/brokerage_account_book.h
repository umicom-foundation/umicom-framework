/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/brokerage_account_book.h
 *
 * PURPOSE:
 *   Maintain bounded brokerage account state owned by the reusable trading core.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_BOOK_H
#define UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_BOOK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/brokerage_account.h"
typedef struct UmiTradingBrokerageAccountBook { UmiTradingBrokerageAccount items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingBrokerageAccountBook;
void umi_trading_brokerage_account_book_init(UmiTradingBrokerageAccountBook *book);
UmiStatus umi_trading_brokerage_account_book_add(UmiTradingBrokerageAccountBook *book,const UmiTradingBrokerageAccount *account);
const UmiTradingBrokerageAccount *umi_trading_brokerage_account_book_find(const UmiTradingBrokerageAccountBook *book,const UmiFinancialId *account_id);
#ifdef __cplusplus
}
#endif
#endif
