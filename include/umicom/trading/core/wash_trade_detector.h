/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/wash_trade_detector.h
 *
 * PURPOSE:
 *   Detect same-beneficial-owner self-cross trade evidence for surveillance review.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_WASH_TRADE_DETECTOR_H
#define UMICOM_TRADING_CORE_WASH_TRADE_DETECTOR_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Flag a trade when buyer and seller beneficial owners are identical and size is material. */
bool umi_trading_wash_trade_detector_detect(const UmiFinancialId *buyer_owner,const UmiFinancialId *seller_owner,UmiTradingQuantityLots lots,UmiTradingQuantityLots minimum_lots);
#ifdef __cplusplus
}
#endif
#endif
