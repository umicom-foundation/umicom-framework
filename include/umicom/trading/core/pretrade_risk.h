/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/pretrade_risk.h
 *
 * PURPOSE:
 *   Apply reusable pre-trade quantity, notional and buying-power gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_PRETRADE_RISK_H
#define UMICOM_TRADING_CORE_PRETRADE_RISK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingPretradeRiskPolicy { UmiTradingQuantityLots max_order_lots; int64_t max_order_notional_minor; } UmiTradingPretradeRiskPolicy;
/* Evaluate order size, notional and current buying power before routing. */
UmiTradingCoreDecision umi_trading_pretrade_risk_check(const UmiTradingPretradeRiskPolicy *policy,UmiTradingQuantityLots lots,int64_t notional_minor,int64_t buying_power_minor);
#ifdef __cplusplus
}
#endif
#endif
