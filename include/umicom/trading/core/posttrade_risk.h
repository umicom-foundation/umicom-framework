/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/posttrade_risk.h
 *
 * PURPOSE:
 *   Evaluate post-fill gross position and daily loss limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_POSTTRADE_RISK_H
#define UMICOM_TRADING_CORE_POSTTRADE_RISK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading posttrade risk policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingPosttradeRiskPolicy { UmiTradingQuantityLots max_abs_position_lots; int64_t max_daily_loss_minor; } UmiTradingPosttradeRiskPolicy;
/* Evaluate absolute position and realised daily loss after fills. */
UmiTradingCoreDecision umi_trading_posttrade_risk_check(const UmiTradingPosttradeRiskPolicy *policy,UmiTradingQuantityLots position_lots,int64_t daily_pnl_minor);
#ifdef __cplusplus
}
#endif
#endif
