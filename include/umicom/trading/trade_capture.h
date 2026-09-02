/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/trade_capture.h
 *
 * PURPOSE:
 *   Create an execution-derived trade identifier string for downstream TMS integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Provide the trade capture reference operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_capture_reference(const UmiExecutionReport *report, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
