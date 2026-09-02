/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/pretrade_risk.h
 *
 * PURPOSE:
 *   Evaluate quantity, notional, projected position and daily loss before an order can proceed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates pretrade risk behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_PRETRADE_RISK_H
#define INCLUDE_UMICOM_TRADING_PRETRADE_RISK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the pretrade risk evaluate operation used by this module and its client
 * applications.
 */
UmiRiskDecision umi_pretrade_risk_evaluate(const UmiOrderRequest *request, const UmiRiskLimit *limit, double current_position, double daily_pnl);
#ifdef __cplusplus
}
#endif
#endif
