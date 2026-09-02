/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_data_event.c
 *
 * PURPOSE:
 *   Normalise venue market-data sequence, instrument identity and event time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_data_event.h"
#include <string.h>
/* Initialise and validate normalise venue market-data sequence, instrument identity and event time.. */
UmiStatus umi_trading_market_data_event_init(UmiTradingMarketDataEvent *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, uint64_t sequence, int64_t event_time_ms) {
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
    if(instrument_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->instrument_id=*instrument_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->sequence=sequence;
    value->event_time_ms=event_time_ms;
    return umi_trading_market_data_event_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_market_data_event_valid(const UmiTradingMarketDataEvent *value) { return value!=NULL && (value->instrument_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->sequence>0U && value->event_time_ms>=0); }
