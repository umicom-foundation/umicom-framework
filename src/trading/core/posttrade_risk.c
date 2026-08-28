/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/posttrade_risk.c
 *
 * PURPOSE:
 *   Evaluate post-fill gross position and daily loss limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/posttrade_risk.h"

static UmiTradingCoreDecision d(bool allowed,const char *reason){UmiTradingCoreDecision x={0};x.allowed=allowed;(void)umi_trading_core_copy_text(x.reason,sizeof x.reason,reason);return x;}
UmiTradingCoreDecision umi_trading_posttrade_risk_check(const UmiTradingPosttradeRiskPolicy *policy,UmiTradingQuantityLots position_lots,int64_t daily_pnl_minor){if(policy==NULL||policy->max_abs_position_lots<=0||policy->max_daily_loss_minor<0)return d(false,"invalid-input");int64_t a=0;if(umi_trading_core_abs_i64(position_lots,&a)!=UMI_STATUS_OK)return d(false,"position-overflow");if(a>policy->max_abs_position_lots)return d(false,"position-limit");if(daily_pnl_minor<0){int64_t loss=0;if(umi_trading_core_abs_i64(daily_pnl_minor,&loss)!=UMI_STATUS_OK||loss>policy->max_daily_loss_minor)return d(false,"daily-loss");}return d(true,"accepted");}
