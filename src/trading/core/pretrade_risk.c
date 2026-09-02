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

/* Provide the d operation used by this module and its client applications. */
static UmiTradingCoreDecision d(bool allowed,const char *reason){UmiTradingCoreDecision x={0};x.allowed=allowed;(void)umi_trading_core_copy_text(x.reason,sizeof x.reason,reason);return x;}
/*
 * Provide the trading pretrade risk check operation used by this module and its client
 * applications.
 */
UmiTradingCoreDecision umi_trading_pretrade_risk_check(const UmiTradingPretradeRiskPolicy *policy,UmiTradingQuantityLots lots,int64_t notional_minor,int64_t buying_power_minor){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||policy->max_order_lots<=0||policy->max_order_notional_minor<=0||lots<=0||notional_minor<0||buying_power_minor<0)return d(false,"invalid-input");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(lots>policy->max_order_lots)return d(false,"quantity-limit");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(notional_minor>policy->max_order_notional_minor)return d(false,"notional-limit");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(notional_minor>buying_power_minor)return d(false,"buying-power");return d(true,"accepted");}
