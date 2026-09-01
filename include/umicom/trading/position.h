/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/position.h
 *
 * PURPOSE:
 *   Apply fills to a position using average-cost accounting for same-direction exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates position behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_POSITION_H
#define INCLUDE_UMICOM_TRADING_POSITION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_position_apply_fill(UmiPosition *position, UmiSide side, double quantity, double price);
#ifdef __cplusplus
}
#endif
#endif
