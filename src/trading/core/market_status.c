/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_status.c
 *
 * PURPOSE:
 *   Capture exchange phase, sequence and operational availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_status.h"
#include <string.h>
/* Initialise and validate capture exchange phase, sequence and operational availability.. */
UmiStatus umi_trading_market_status_init(UmiTradingMarketStatus *value,UmiTradingCoreMarketPhase phase, uint64_t sequence, bool operational) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->phase=phase;
    value->sequence=sequence;
    value->operational=operational;
    return umi_trading_market_status_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_market_status_valid(const UmiTradingMarketStatus *value) { return value!=NULL && (value->phase>=UMI_TRADING_CORE_PHASE_CLOSED && value->phase<=UMI_TRADING_CORE_PHASE_POSTCLOSE && value->sequence>0U); }
