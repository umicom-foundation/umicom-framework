/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/commission_calculation.c
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
#include "umicom/trading/core/commission_calculation.h"

#include <limits.h>
/* Calculate commission minor units while detecting signed overflow. */
UmiStatus umi_trading_commission_calculation_compute(const UmiTradingCommissionSchedule *schedule,UmiTradingQuantityLots lots,int64_t *out_minor){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_trading_commission_schedule_valid(schedule)||out_minor==NULL||lots<=0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(schedule->per_lot_minor!=0&&lots>INT64_MAX/schedule->per_lot_minor)return UMI_STATUS_CAPACITY_EXCEEDED;int64_t fee=lots*schedule->per_lot_minor;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(fee<schedule->minimum_minor)fee=schedule->minimum_minor;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(schedule->maximum_minor>0&&fee>schedule->maximum_minor)fee=schedule->maximum_minor;*out_minor=fee;return UMI_STATUS_OK;}
