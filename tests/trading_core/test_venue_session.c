/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_session.c
 *
 * PURPOSE:
 *   Exercise model a bounded venue trading session and its current phase.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"v1")!=UMI_STATUS_OK) return 9;
     UmiTradingVenueSession v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_venue_session_init(&v,&id,1000,2000,UMI_TRADING_CORE_PHASE_CONTINUOUS)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_venue_session_valid(&v)) return 2;
     return 0;
}
