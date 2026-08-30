/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/fee_calculation.h
 *
 * PURPOSE:
 *   Calculate maker or taker exchange fees plus regulatory charges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_FEE_CALCULATION_H
#define UMICOM_TRADING_CORE_FEE_CALCULATION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/fee_schedule.h"
/* Compute integer fee minor units for a fill. */
UmiStatus umi_trading_fee_calculation_compute(const UmiTradingFeeSchedule *schedule,UmiTradingQuantityLots lots,bool maker,int64_t *out_minor);
#ifdef __cplusplus
}
#endif
#endif
