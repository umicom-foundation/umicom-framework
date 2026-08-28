/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/fee_calculation.c
 *
 * PURPOSE:
 *   Calculate maker or taker exchange fees plus regulatory charges.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_calculation.h"

#include <limits.h>
UmiStatus umi_trading_fee_calculation_compute(const UmiTradingFeeSchedule *schedule,UmiTradingQuantityLots lots,bool maker,int64_t *out_minor){if(!umi_trading_fee_schedule_valid(schedule)||lots<=0||out_minor==NULL)return UMI_STATUS_INVALID_ARGUMENT;int64_t rate=0;int64_t base=maker?schedule->maker_minor_per_lot:schedule->taker_minor_per_lot;if(umi_trading_core_add_i64(base,schedule->regulatory_minor_per_lot,&rate)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(rate<0||(rate!=0&&lots>INT64_MAX/rate))return UMI_STATUS_CAPACITY_EXCEEDED;*out_minor=rate*lots;return UMI_STATUS_OK;}
