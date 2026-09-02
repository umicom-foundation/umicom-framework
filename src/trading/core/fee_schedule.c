/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/fee_schedule.c
 *
 * PURPOSE:
 *   Define maker/taker exchange fees in minor units per lot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_schedule.h"
#include <string.h>
/* Initialise and validate define maker/taker exchange fees in minor units per lot.. */
UmiStatus umi_trading_fee_schedule_init(UmiTradingFeeSchedule *value,int64_t maker_minor_per_lot, int64_t taker_minor_per_lot, int64_t regulatory_minor_per_lot) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->maker_minor_per_lot=maker_minor_per_lot;
    value->taker_minor_per_lot=taker_minor_per_lot;
    value->regulatory_minor_per_lot=regulatory_minor_per_lot;
    return umi_trading_fee_schedule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_fee_schedule_valid(const UmiTradingFeeSchedule *value) { return value!=NULL && (value->maker_minor_per_lot>=0 && value->taker_minor_per_lot>=0 && value->regulatory_minor_per_lot>=0); }
