/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_session.c
 *
 * PURPOSE:
 *   Model a bounded venue trading session and its current phase.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_session.h"
#include <string.h>
/* Initialise and validate model a bounded venue trading session and its current phase.. */
UmiStatus umi_trading_venue_session_init(UmiTradingVenueSession *value,const UmiFinancialId * venue_id, int64_t open_time_ms, int64_t close_time_ms, UmiTradingCoreMarketPhase phase) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->open_time_ms=open_time_ms;
    value->close_time_ms=close_time_ms;
    value->phase=phase;
    return umi_trading_venue_session_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_venue_session_valid(const UmiTradingVenueSession *value) { return value!=NULL && (value->venue_id.value[0]!='\0' && value->open_time_ms>=0 && value->close_time_ms>value->open_time_ms && value->phase>=UMI_TRADING_CORE_PHASE_CLOSED && value->phase<=UMI_TRADING_CORE_PHASE_POSTCLOSE); }
