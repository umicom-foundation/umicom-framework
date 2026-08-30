/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/commission_schedule.c
 *
 * PURPOSE:
 *   Define per-lot and minimum brokerage commission in integer minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/commission_schedule.h"
#include <string.h>
/* Initialise and validate define per-lot and minimum brokerage commission in integer minor units.. */
UmiStatus umi_trading_commission_schedule_init(UmiTradingCommissionSchedule *value,int64_t per_lot_minor, int64_t minimum_minor, int64_t maximum_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->per_lot_minor=per_lot_minor;
    value->minimum_minor=minimum_minor;
    value->maximum_minor=maximum_minor;
    return umi_trading_commission_schedule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_commission_schedule_valid(const UmiTradingCommissionSchedule *value) { return value!=NULL && (value->per_lot_minor>=0 && value->minimum_minor>=0 && value->maximum_minor>=value->minimum_minor); }
