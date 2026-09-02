/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/position_ledger.h
 *
 * PURPOSE:
 *   Maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_POSITION_LEDGER_H
#define UMICOM_TRADING_CORE_POSITION_LEDGER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading position ledger entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingPositionLedgerEntry { UmiFinancialId instrument_id; UmiTradingQuantityLots lots; } UmiTradingPositionLedgerEntry;
/**
 * Represent the trading position ledger data shared with callers of this public contract.
 */
typedef struct UmiTradingPositionLedger { UmiTradingPositionLedgerEntry items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingPositionLedger;
/**
 * Initialise trading position ledger from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_position_ledger_init(UmiTradingPositionLedger *ledger);
/**
 * Perform trading position ledger through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_trading_position_ledger_apply(UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id,UmiSide side,UmiTradingQuantityLots lots);
/**
 * Provide the trading position ledger get operation used by this module and its client
 * applications.
 */
UmiTradingQuantityLots umi_trading_position_ledger_get(const UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id);
#ifdef __cplusplus
}
#endif
#endif
