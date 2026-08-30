/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/commission_calculation.h
 *
 * PURPOSE:
 *   Calculate bounded per-lot brokerage commission from a commission schedule.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_COMMISSION_CALCULATION_H
#define UMICOM_TRADING_CORE_COMMISSION_CALCULATION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/commission_schedule.h"
/* Calculate commission minor units while detecting signed overflow. */
UmiStatus umi_trading_commission_calculation_compute(const UmiTradingCommissionSchedule *schedule,UmiTradingQuantityLots lots,int64_t *out_minor);
#ifdef __cplusplus
}
#endif
#endif
