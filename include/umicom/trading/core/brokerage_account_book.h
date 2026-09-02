/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/brokerage_account_book.h
 *
 * PURPOSE:
 *   Maintain bounded brokerage account state owned by the reusable trading core.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_BOOK_H
#define UMICOM_TRADING_CORE_BROKERAGE_ACCOUNT_BOOK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/brokerage_account.h"
/**
 * Represent the trading brokerage account book data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingBrokerageAccountBook { UmiTradingBrokerageAccount items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingBrokerageAccountBook;
/**
 * Initialise trading brokerage account book from caller-provided values so later
 * operations receive a known state.
 */
void umi_trading_brokerage_account_book_init(UmiTradingBrokerageAccountBook *book);
/**
 * Add trading brokerage account book only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_trading_brokerage_account_book_add(UmiTradingBrokerageAccountBook *book,const UmiTradingBrokerageAccount *account);
/**
 * Find trading brokerage account book while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTradingBrokerageAccount *umi_trading_brokerage_account_book_find(const UmiTradingBrokerageAccountBook *book,const UmiFinancialId *account_id);
#ifdef __cplusplus
}
#endif
#endif
