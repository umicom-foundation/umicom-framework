/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/risk_decision.h
 *
 * PURPOSE:
 *   Create allow/deny risk decisions with bounded human-readable reasons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates risk decision behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_RISK_DECISION_H
#define INCLUDE_UMICOM_TRADING_RISK_DECISION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
void umi_risk_decision_allow(UmiRiskDecision *decision);
void umi_risk_decision_deny(UmiRiskDecision *decision, const char *reason);
#ifdef __cplusplus
}
#endif
#endif
