/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/cash.h
 *
 * PURPOSE:
 *   Apply minor-unit cash movements to a same-currency balance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates cash behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_CASH_H
#define INCLUDE_UMICOM_TRADING_CASH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_cash_apply(UmiMoney *balance, const UmiMoney *movement);
#ifdef __cplusplus
}
#endif
#endif
