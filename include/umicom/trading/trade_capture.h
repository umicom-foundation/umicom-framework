/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/trade_capture.h
 *
 * PURPOSE:
 *   Create an execution-derived trade identifier string for downstream TMS integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates trade capture behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_TRADE_CAPTURE_H
#define INCLUDE_UMICOM_TRADING_TRADE_CAPTURE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_trade_capture_reference(const UmiExecutionReport *report, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
