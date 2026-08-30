/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_integrity_snapshot.c
 *
 * PURPOSE:
 *   Exercise aggregate exchange integrity indicators for operational and surveillance dashboards.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_integrity_snapshot.h"
int main(void) {

    UmiTradingMarketIntegritySnapshot s={0};
    umi_trading_market_integrity_snapshot_update(&s,1U,0U,0U,100U);
    return s.severity==UMI_TRADING_CORE_WARNING?0:1;
}
