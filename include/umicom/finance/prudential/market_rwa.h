/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/market_rwa.h
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MARKET_RWA_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MARKET_RWA_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential market rwa data shared with callers of this public contract.
 */
typedef struct UmiPrudentialMarketRwa { double capital_charge; double conversion_factor; double rwa; } UmiPrudentialMarketRwa;
/* Convert a capital charge to RWA using a positive reciprocal minimum-ratio factor. */
UmiStatus umi_pru_market_rwa_calculate(UmiPrudentialMarketRwa *result, double capital_charge, double conversion_factor);

#ifdef __cplusplus
}
#endif

#endif
