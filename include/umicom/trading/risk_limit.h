/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/risk_limit.h
 *
 * PURPOSE:
 *   Validate risk-limit configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates risk limit behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_RISK_LIMIT_H
#define INCLUDE_UMICOM_TRADING_RISK_LIMIT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_risk_limit_valid(const UmiRiskLimit *limit);
#ifdef __cplusplus
}
#endif
#endif
