/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_type.h
 *
 * PURPOSE:
 *   Describe whether an order type requires limit or stop prices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates order type behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_TYPE_H
#define INCLUDE_UMICOM_TRADING_ORDER_TYPE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_order_type_requires_limit(UmiOrderType type);
int umi_order_type_requires_stop(UmiOrderType type);
#ifdef __cplusplus
}
#endif
#endif
