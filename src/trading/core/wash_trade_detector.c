/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/wash_trade_detector.c
 *
 * PURPOSE:
 *   Detect same-beneficial-owner self-cross trade evidence for surveillance review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/wash_trade_detector.h"

bool umi_trading_wash_trade_detector_detect(const UmiFinancialId *buyer_owner,const UmiFinancialId *seller_owner,UmiTradingQuantityLots lots,UmiTradingQuantityLots minimum_lots){return buyer_owner!=NULL&&seller_owner!=NULL&&lots>=minimum_lots&&minimum_lots>0&&umi_trading_core_id_equal(buyer_owner,seller_owner);}
