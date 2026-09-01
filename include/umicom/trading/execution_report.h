/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/execution_report.h
 *
 * PURPOSE:
 *   Validate fill/execution reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates execution report behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_EXECUTION_REPORT_H
#define INCLUDE_UMICOM_TRADING_EXECUTION_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_execution_report_valid(const UmiExecutionReport *report);
#ifdef __cplusplus
}
#endif
#endif
