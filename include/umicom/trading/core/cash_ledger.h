/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/cash_ledger.h
 *
 * PURPOSE:
 *   Maintain bounded integer cash balances by currency for brokerage settlement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_CASH_LEDGER_H
#define UMICOM_TRADING_CORE_CASH_LEDGER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading cash ledger entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingCashLedgerEntry { UmiCurrency currency; int64_t minor_units; } UmiTradingCashLedgerEntry;
/**
 * Represent the trading cash ledger data shared with callers of this public contract.
 */
typedef struct UmiTradingCashLedger { UmiTradingCashLedgerEntry items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingCashLedger;
/**
 * Initialise trading cash ledger from caller-provided values so later operations receive a
 * known state.
 */
void umi_trading_cash_ledger_init(UmiTradingCashLedger *ledger);
/**
 * Perform trading cash ledger through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_trading_cash_ledger_apply(UmiTradingCashLedger *ledger,const UmiCurrency *currency,int64_t delta_minor);
/**
 * Provide the trading cash ledger get operation used by this module and its client
 * applications.
 */
int64_t umi_trading_cash_ledger_get(const UmiTradingCashLedger *ledger,const UmiCurrency *currency);
#ifdef __cplusplus
}
#endif
#endif
