/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_session.c
 *
 * PURPOSE:
 *   Exercise model a bounded venue trading session and its current phase.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_session.h"
int main(void) {
    UmiFinancialId id;
     if(umi_trading_core_id_assign(&id,"v1")!=UMI_STATUS_OK) return 9;
     UmiTradingVenueSession v;
     if(umi_trading_venue_session_init(&v,&id,1000,2000,UMI_TRADING_CORE_PHASE_CONTINUOUS)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_venue_session_valid(&v)) return 2;
     return 0;
}
