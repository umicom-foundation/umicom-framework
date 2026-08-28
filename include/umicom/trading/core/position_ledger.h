/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/position_ledger.h
 *
 * PURPOSE:
 *   Maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_POSITION_LEDGER_H
#define UMICOM_TRADING_CORE_POSITION_LEDGER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingPositionLedgerEntry { UmiFinancialId instrument_id; UmiTradingQuantityLots lots; } UmiTradingPositionLedgerEntry;
typedef struct UmiTradingPositionLedger { UmiTradingPositionLedgerEntry items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingPositionLedger;
void umi_trading_position_ledger_init(UmiTradingPositionLedger *ledger);
UmiStatus umi_trading_position_ledger_apply(UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id,UmiSide side,UmiTradingQuantityLots lots);
UmiTradingQuantityLots umi_trading_position_ledger_get(const UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id);
#ifdef __cplusplus
}
#endif
#endif
