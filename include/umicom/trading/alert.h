/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/alert.h
 *
 * PURPOSE:
 *   Evaluate whether a value crosses a configured threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates alert behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ALERT_H
#define INCLUDE_UMICOM_TRADING_ALERT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_trading_alert_crossed(double previous_value, double current_value, double threshold);
#ifdef __cplusplus
}
#endif
#endif
