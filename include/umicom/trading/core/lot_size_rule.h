/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/lot_size_rule.h
 *
 * PURPOSE:
 *   Represent minimum and step quantities for a venue listing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_LOT_SIZE_RULE_H
#define UMICOM_TRADING_CORE_LOT_SIZE_RULE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading lot size rule data shared with callers of this public contract.
 */
typedef struct UmiTradingLotSizeRule { UmiTradingQuantityLots minimum_lots; UmiTradingQuantityLots step_lots; UmiTradingQuantityLots maximum_lots; } UmiTradingLotSizeRule;
/* Initialise and validate represent minimum and step quantities for a venue listing. */
UmiStatus umi_trading_lot_size_rule_init(UmiTradingLotSizeRule *value,UmiTradingQuantityLots minimum_lots, UmiTradingQuantityLots step_lots, UmiTradingQuantityLots maximum_lots);
/* Validate the invariant set for this trading record. */
bool umi_trading_lot_size_rule_valid(const UmiTradingLotSizeRule *value);
#ifdef __cplusplus
}
#endif
#endif
