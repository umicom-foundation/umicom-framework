/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/market/quote_spread.h
 *
 * PURPOSE:
 *   Evaluate Quote Spread for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_MARKET_QUOTE_SPREAD_H
#define UMICOM_TRADING_WORKSTATION_MARKET_QUOTE_SPREAD_H

#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_market_quote_spread(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
