/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/pretrade_risk.c
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
#include "umicom/trading/core/pretrade_risk.h"

static UmiTradingCoreDecision d(bool allowed,const char *reason){UmiTradingCoreDecision x={0};x.allowed=allowed;(void)umi_trading_core_copy_text(x.reason,sizeof x.reason,reason);return x;}
UmiTradingCoreDecision umi_trading_pretrade_risk_check(const UmiTradingPretradeRiskPolicy *policy,UmiTradingQuantityLots lots,int64_t notional_minor,int64_t buying_power_minor){if(policy==NULL||policy->max_order_lots<=0||policy->max_order_notional_minor<=0||lots<=0||notional_minor<0||buying_power_minor<0)return d(false,"invalid-input");if(lots>policy->max_order_lots)return d(false,"quantity-limit");if(notional_minor>policy->max_order_notional_minor)return d(false,"notional-limit");if(notional_minor>buying_power_minor)return d(false,"buying-power");return d(true,"accepted");}
