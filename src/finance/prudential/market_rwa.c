/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/market_rwa.c
 *
 * PURPOSE:
 *   Convert a market-risk capital charge into risk-weighted assets.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/market_rwa.h"

/*
 * Provide the pru market rwa calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_pru_market_rwa_calculate(UmiPrudentialMarketRwa *result,double capital_charge,double conversion_factor) { double rwa; UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||conversion_factor<=0.0)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(capital_charge,conversion_factor,&rwa); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; result->capital_charge=capital_charge; result->conversion_factor=conversion_factor; result->rwa=rwa; return UMI_STATUS_OK; }
