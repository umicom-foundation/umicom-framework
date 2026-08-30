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

typedef struct UmiTradingCashLedgerEntry { UmiCurrency currency; int64_t minor_units; } UmiTradingCashLedgerEntry;
typedef struct UmiTradingCashLedger { UmiTradingCashLedgerEntry items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingCashLedger;
void umi_trading_cash_ledger_init(UmiTradingCashLedger *ledger);
UmiStatus umi_trading_cash_ledger_apply(UmiTradingCashLedger *ledger,const UmiCurrency *currency,int64_t delta_minor);
int64_t umi_trading_cash_ledger_get(const UmiTradingCashLedger *ledger,const UmiCurrency *currency);
#ifdef __cplusplus
}
#endif
#endif
