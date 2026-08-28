/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_replace.h
 *
 * PURPOSE:
 *   Describe a replacement request with optimistic order-version control.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_REPLACE_H
#define UMICOM_TRADING_CORE_ORDER_REPLACE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingOrderReplace { UmiFinancialId client_order_id; uint64_t expected_version; UmiTradingQuantityLots new_quantity_lots; UmiTradingPriceTicks new_limit_ticks; } UmiTradingOrderReplace;
/* Initialise and validate describe a replacement request with optimistic order-version control. */
UmiStatus umi_trading_order_replace_init(UmiTradingOrderReplace *value,const UmiFinancialId * client_order_id, uint64_t expected_version, UmiTradingQuantityLots new_quantity_lots, UmiTradingPriceTicks new_limit_ticks);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_replace_valid(const UmiTradingOrderReplace *value);
#ifdef __cplusplus
}
#endif
#endif
