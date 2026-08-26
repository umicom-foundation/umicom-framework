/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_market_rwa.c
 *
 * PURPOSE:
 *   Verify convert a market-risk capital charge into risk-weighted assets.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/market_rwa.h"

int main(void) { UmiPrudentialMarketRwa r; if(umi_pru_market_rwa_calculate(&r,8.0,12.5)!=UMI_STATUS_OK)return 1; return r.rwa==100.0?0:2; }
